//
// Created by Artyom Dangizyan on 10/8/18.
//

#pragma once


class IndexBuffer {
private:
    unsigned int m_RendererId;
    unsigned int m_Count;
public:
    IndexBuffer(const void *data, unsigned int count);

    ~IndexBuffer();

    void bind() const ;

    void unbind() const ;

    inline unsigned int getCount() const {
        return m_Count;
    }

};
