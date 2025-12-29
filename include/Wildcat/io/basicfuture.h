//
// Created by hstasonis on 12/28/25.
//

#pragma once
#include <mutex>
#include <string>

class SimpleFuture
{
public:
    SimpleFuture();

    [[nodiscard]] bool isCompleted();

    void setCompleted(bool value, const std::string &result);

    [[nodiscard]] std::string getValue() const
    {
        return m_result;
    }

private:
    std::mutex m_lock;

    bool mb_completed = false;
    std::string m_result;
};
