#pragma once

class ChineseChar
{
public:
    explicit ChineseChar(char firstHalve, char secondHalve)
    {
        mData[0] = firstHalve;
        mData[1] = secondHalve;
    }

    const char& GetFirstHalve() const
    {
        return mData[0];
    }

    const char& GetSecondHalve() const
    {
        return mData[1];
    }
    
    bool operator==(const ChineseChar& other) const
    {
        return GetFirstHalve()  == other.GetFirstHalve()
            && GetSecondHalve() == other.GetSecondHalve(); 
    }
    
    static size_t GetHashCode(const ChineseChar& theChar)
    {
        return (theChar.GetFirstHalve() + theChar.GetSecondHalve())
            * (theChar.GetFirstHalve() - theChar.GetSecondHalve());
    }

private:
    char mData[2];
};
