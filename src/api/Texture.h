//
// Created by Artyom Dangizyan on 11/14/18.
//
#pragma once

#include <string>

class Texture {
private:
    unsigned int m_RendererId;
    std::string m_FilePath;
    unsigned char *m_LocalBuffer;
    int m_Width, m_Height, m_BPP;
public:
    Texture(const std::string &filePath);

    ~Texture();

    void bind(unsigned int slot = 0);

    void unbind();

    inline int getM_Width() const {
        return m_Width;
    }

    inline int getM_Height() const {
        return m_Height;
    }
};
