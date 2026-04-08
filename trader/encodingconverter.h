#include <cstring>
#include <string>
#include <vector>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <iconv.h>
#endif

class EncodingConverter
{
public:
    // 检测是否为 GBK/GB2312 编码
    static bool isGbk(const char *str)
    {
        if (str == nullptr)
            return false;

        for (size_t i = 0; str[i] != '\0';)
        {
            uint8_t c = static_cast<uint8_t>(str[i]);

            // ASCII 范围 (0x00-0x7F)
            if (c <= 0x7F)
            {
                ++i;
                continue;
            }

            // GBK 双字节: 第一个字节 0x81-0xFE
            if (c < 0x81 || c > 0xFE)
            {
                return false; // 超出 GBK 首字节范围
            }

            // 需要第二个字节
            if (str[i + 1] == '\0')
            {
                return false; // 截断的字节
            }

            uint8_t c2 = static_cast<uint8_t>(str[i + 1]);

            // GBK 第二个字节: 0x40-0xFE (不包括 0x7F)
            if (c2 < 0x40 || c2 > 0xFE || c2 == 0x7F)
            {
                return false; // 无效的 GBK 第二个字节
            }

            i += 2;
        }
        return true;
    }

    // 检测是否为纯 ASCII
    static bool isAscii(const char *str)
    {
        if (str == nullptr)
            return false;

        for (size_t i = 0; str[i] != '\0'; ++i)
        {
            uint8_t c = static_cast<uint8_t>(str[i]);
            if (c > 0x7F)
                return false;
        }
        return true;
    }

    // 检测是否为有效的 UTF-8
    static bool isUtf8(const char *str)
    {
        if (str == nullptr)
            return false;

        for (size_t i = 0; str[i] != '\0';)
        {
            uint8_t c = static_cast<uint8_t>(str[i]);

            // 单字节 ASCII
            if ((c & 0x80) == 0)
            {
                ++i;
                continue;
            }

            // 多字节序列长度判断
            int bytes;
            if ((c & 0xE0) == 0xC0)
                bytes = 2; // 110xxxxx
            else if ((c & 0xF0) == 0xE0)
                bytes = 3; // 1110xxxx
            else if ((c & 0xF8) == 0xF0)
                bytes = 4; // 11110xxx
            else
                return false; // 无效的起始字节

            // 检查后续字节 (10xxxxxx)
            for (int j = 1; j < bytes; ++j)
            {
                if (str[i + j] == '\0')
                    return false;
                uint8_t next = static_cast<uint8_t>(str[i + j]);
                if ((next & 0xC0) != 0x80)
                    return false;
            }

            i += bytes;
        }
        return true;
    }

    // GBK 转 UTF-8
    static std::string gbkToUtf8(const char *gbkStr)
    {
#ifdef _WIN32
        return gbkToUtf8Win32(gbkStr);
#else
        return convertWithIconv(gbkStr, "GBK", "UTF-8");
#endif
    }

    // UTF-8 转 GBK
    static std::string utf8ToGbk(const char *utf8Str)
    {
#ifdef _WIN32
        return utf8ToGbkWin32(utf8Str);
#else
        return convertWithIconv(utf8Str, "UTF-8", "GBK");
#endif
    }

private:
#ifdef _WIN32
    // Windows 实现
    static std::string gbkToUtf8Win32(const char *gbkStr)
    {
        if (gbkStr == nullptr || gbkStr[0] == '\0')
            return "";

        // GBK -> UTF-16
        int utf16Len = MultiByteToWideChar(CP_ACP, 0, gbkStr, -1, nullptr, 0);
        if (utf16Len <= 0)
            throw std::runtime_error("GBK to UTF-16 conversion failed");

        std::vector<wchar_t> utf16(utf16Len);
        MultiByteToWideChar(CP_ACP, 0, gbkStr, -1, utf16.data(), utf16Len);

        // UTF-16 -> UTF-8
        int utf8Len = WideCharToMultiByte(CP_UTF8, 0, utf16.data(), -1, nullptr, 0, nullptr, nullptr);
        if (utf8Len <= 0)
            throw std::runtime_error("UTF-16 to UTF-8 conversion failed");

        std::vector<char> utf8(utf8Len);
        WideCharToMultiByte(CP_UTF8, 0, utf16.data(), -1, utf8.data(), utf8Len, nullptr, nullptr);

        return std::string(utf8.data());
    }

    static std::string utf8ToGbkWin32(const char *utf8Str)
    {
        if (utf8Str == nullptr || utf8Str[0] == '\0')
            return "";

        // UTF-8 -> UTF-16
        int utf16Len = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, nullptr, 0);
        if (utf16Len <= 0)
            throw stdruntime_error("UTF-8 to UTF-16 conversion failed");

        std::vector<wchar_t> utf16(utf16Len);
        MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, utf16.data(), utf16Len);

        // UTF-16 -> GBK
        int gbkLen = WideCharToMultiByte(CP_ACP, 0, utf16.data(), -1, nullptr, 0, nullptr, nullptr);
        if (gbkLen <= 0)
            throw std::runtime_error("UTF-16 to GBK conversion failed");

        std::vector<char> gbk(gbkLen);
        WideCharToMultiByte(CP_ACP, 0, utf16.data(), -1, gbk.data(), gbkLen, nullptr, nullptr);

        return std::string(gbk.data());
    }

#else
    // Linux/macOS 实现
    static std::string convertWithIconv(const char *input,
                                        const char *fromEncoding,
                                        const char *toEncoding)
    {
        if (input == nullptr || input[0] == '\0')
            return "";

        iconv_t cd = iconv_open(toEncoding, fromEncoding);
        if (cd == (iconv_t)-1)
        {
            throw std::runtime_error(std::string("iconv_open failed: ") +
                                     (errno == EINVAL ? "unsupported encoding" : "unknown error"));
        }

        // iconv 会修改输入指针，需要复制
        size_t inLen = strlen(input);
        char *inBuf = const_cast<char *>(input);

        // 输出缓冲区（UTF-8 通常不会超过 GBK 的 2 倍）
        size_t outLen = inLen * 4;
        std::vector<char> outBuf(outLen);
        char *outPtr = outBuf.data();

        size_t originalOutLen = outLen;

        if (iconv(cd, &inBuf, &inLen, &outPtr, &outLen) == (size_t)-1)
        {
            iconv_close(cd);
            throw std::runtime_error(std::string("iconv conversion failed, errno: ") + std::to_string(errno));
        }

        iconv_close(cd);

        // 计算实际输出长度
        size_t convertedLen = originalOutLen - outLen;
        return std::string(outBuf.data(), convertedLen);
    }
#endif
};
