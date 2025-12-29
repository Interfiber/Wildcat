//
// Created by hstasonis on 12/28/25.
//

#include <Wildcat/io/basicfuture.h>

SimpleFuture::SimpleFuture()
{
    mb_completed = false;
}

bool SimpleFuture::isCompleted()
{
    std::lock_guard lk(m_lock);

    return mb_completed;
}

void SimpleFuture::setCompleted(const bool value, const std::string &result)
{
    std::lock_guard lk(m_lock);

    mb_completed = value;
    m_result = result;
}

