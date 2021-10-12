#include <logs.h>

String strlog = "";

RingBuffer::RingBuffer(int cap) : buffer(cap), first(0), last(0), sz(0) {}

bool RingBuffer::empty() const { return sz == 0; }
bool RingBuffer::full() const { return sz == buffer.size(); }

void RingBuffer::push(String str)
{
    if (last >= buffer.size())
        last = 0;
    buffer[last] = str;
    ++last;
    if (full())
        first = (first + 1) % buffer.size();
    else
        ++sz;
}
void RingBuffer::print() const
{
    strlog = "";
    if (first < last)
        for (size_t i = first; i < last; ++i)
        {
            strlog += (buffer[i] + "<br>");
        }
    else
    {
        for (size_t i = first; i < buffer.size(); ++i)
        {
            strlog += (buffer[i] + "<br>");
        }
        for (size_t i = 0; i < last; ++i)
        {
            strlog += (buffer[i] + "<br>");
        }
    }
}
RingBuffer circle(10);

void logOutput(String string1)
{
    circle.push(string1);
    Serial.println(string1);
}