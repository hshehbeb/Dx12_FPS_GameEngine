#pragma once
#include <unordered_map>

#include "../Core/Log.h"

template <class t_Value>
class EfficientLookup
{
public:
    EfficientLookup() = default;
    EfficientLookup(EfficientLookup& other) = delete;
    EfficientLookup(EfficientLookup&& other) = default;

    bool ContainsKey(std::string key)
    {
        // bool found = std::find(mRegistry.begin(), mRegistry.end(), key) != mRegistry.end();
        bool found = (mRegistry.find(key) != mRegistry.end());
        return found;
    }
    
    void Add(const std::string& key, t_Value value)
    {
        if (ContainsKey(key))
            Log::Debug(std::string("[EffLookup] WARNING, already has a key [") + key + "] registered");

        mRegistry[key] = value;
        mKeys.push_back(key);
        mValues.push_back(value);
    }

    /**
     * will generate a random string as key
     * which means you will never able to reference that again
     */
    void Add(t_Value value)
    {
        std::string randId = std::to_string(rand());
        while (ContainsKey(randId))
            randId = std::to_string(rand());

        Add(randId, value);
    }

    t_Value& Get(std::string key)
    {
        if (!ContainsKey(key))
            throw std::invalid_argument(key + "is not found in this EffLookup");
        
        return mRegistry[key];
    }

    const std::vector<t_Value>& GetValues()
    {
        return mValues;
    }

    const std::vector<std::string>& GetKeys()
    {
        return mKeys;
    }
    
private:
    std::unordered_map<std::string, t_Value> mRegistry;
    std::vector<t_Value> mValues;
    std::vector<std::string> mKeys;
};
