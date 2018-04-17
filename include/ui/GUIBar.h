/* 
 * The GUIBar class provides a thin wrapper around an AntTweakBar bar.
 *
 * Instead of passing around pointers for display/manipulation of fields,
 * the GUIBar class promotes encapsulation by enabling easy binding of getters
 * and setters via an intuitive, templatized std::function interface.
 *
 * @author Philip Salzmann
 */

#pragma once

#include <AntTweakBar.h>
#include <functional>
#include <vector>
#include <memory>
#include <sstream>
#include <map>
#include <limits>
#include <cmath>

template <typename T>
using Getter = std::function<T()>;

template <typename T>
using Setter = std::function<void(T)>;

typedef std::function<void()> Callback;

class GUIBarOptions {
public:

    GUIBarOptions(std::string fieldLabel)
    : mFieldLabel(fieldLabel)
    , mMinValue(std::numeric_limits<float>::infinity())
    , mMaxValue(std::numeric_limits<float>::infinity())
    , mStepSize(std::numeric_limits<float>::infinity()) {
    }

    GUIBarOptions& group(std::string value) {
        mGroup = value;
        return *this;
    }

    GUIBarOptions& min(float value) {
        mMinValue = value;
        return *this;
    }

    GUIBarOptions& max(float value) {
        mMaxValue = value;
        return *this;
    }

    GUIBarOptions& step(float value) {
        mStepSize = value;
        return *this;
    }

    std::string getLabel() {
        return mFieldLabel;
    }

    std::string getGroup() {
        return mGroup;
    }

    std::string asString() {
        std::stringstream ss;
        if (!mGroup.empty()) {
            ss << " group='" << mGroup << "' ";
        }

        if (!std::isinf(mMinValue)) {
            ss << " min=" << mMinValue << " ";
        }

        if (!std::isinf(mMaxValue)) {
            ss << " max=" << mMaxValue << " ";
        }

        if (!std::isinf(mStepSize)) {
            ss << " step=" << mStepSize << " ";
        }

        return ss.str();
    }

private:
    std::string mFieldLabel, mGroup;
    float mMinValue, mMaxValue, mStepSize;
};

class GUIBar {
    // The ToTwType trait enables easy getter/setter binding for GUI fields
    // NOTE: AntTweakBar supports some more types, so add those below if you need them.

    template<typename T>
    struct ToTwType {static const ETwType value = TW_TYPE_UNDEF;
    };

public:
    GUIBar();
    virtual ~GUIBar();

    void init();
    void setTitle(std::string title);

    // See setter version below for more information.

    template <typename T>
    void addField(GUIBarOptions options, Getter<T> getter) {
        static_assert(ToTwType<T>::value != TW_TYPE_UNDEF, "Unsupported field type");

        auto getterCB = [](void* value, void* clientData) {
            *static_cast<T*> (value) = (*(*static_cast<GSCallbackContainer<T>*> (clientData)).getter())();
        };

        // create and store a copy of the functor to ensure its validity after leaving the scope
        auto container = std::make_shared<GSCallbackContainer < T >> (std::make_shared<Getter < T >> (getter), nullptr);
        TwAddVarCB(mTwBar, options.getLabel().c_str(), ToTwType<T>::value, nullptr, getterCB, container.get(), options.asString().c_str());
        mGSCallbacks.push_back(container);

        storeControl(options.getLabel(), options.getGroup());
    }

    // Adds a new field with getter and setter functions.
    // As type inference doesn't always work here, it is recommended to use the specialized versions of the function instead.
    //
    // Example usage with lambdas:
    //
    //      auto myGetter = []() { return 42; };
    //      auto mySetter = [](int value) { ... };
    //      addField<int>(..., myGetter, mySetter, ...);
    //
    // Example usage with member functions:
    //
    //      addField<float>(..., std::bind(&MyClass::myGetter, &myClassInstance),
    //                           std::bind(&MyClass::mySetter, &myClassInstance, std::placeholders::_1), ...);
    //
    // Ensure that when using std::bind, a pointer to the instance is given.
    //

    template <typename T>
    void addField(GUIBarOptions options, Getter<T> getter, Setter<T> setter) {
        static_assert(ToTwType<T>::value != TW_TYPE_UNDEF, "Unsupported field type");

        auto getterCB = [](void* value, void* clientData) {
            *static_cast<T*> (value) = (*(*static_cast<GSCallbackContainer<T>*> (clientData)).getter())();
        };

        auto setterCB = [](const void* value, void* clientData) {
            (*(*static_cast<GSCallbackContainer<T>*> (clientData)).setter())(*static_cast<const T*> (value));
        };

        // create and store a copy of the functor to ensure its validity after leaving the scope
        auto container = std::make_shared<GSCallbackContainer < T >> (std::make_shared<Getter < T >> (getter), std::make_shared<Setter < T >> (setter));
        TwAddVarCB(mTwBar, options.getLabel().c_str(), ToTwType<T>::value, setterCB, getterCB, container.get(), options.asString().c_str());
        mGSCallbacks.push_back(container);

        storeControl(options.getLabel(), options.getGroup());
    }

    void addButton(GUIBarOptions options, Callback callback);

    // wraps a raw addVarCB call. unfortunately still needed for things such as enums
    void addVarCBRaw(GUIBarOptions options, TwType type, TwSetVarCallback setter, TwGetVarCallback getter, void* clientData);

    void closeGroup(std::string name);
    void setSize(unsigned w, unsigned h) const;
    void setPosition(unsigned x, unsigned y) const;
    // remove all controls
    void reset();
private:
    // returns a unique name for the TwBar
    static std::string getUniqueName();
    // wrapper for TwDefine
    void define(std::string option) const;
    void storeControl(std::string label, std::string group);
    // resize window based on fields and groups
    void resize();

    TwBar* mTwBar;
    std::string mTwBarName;

    // CALLBACK HANDLING

    // the idea here is to keep a copy of the functor passed as getter / setter
    // in memory via the vector below. 

    class GSCallbackContainerRaw {
    public:

        virtual ~GSCallbackContainerRaw() {
        };
    };

    // note that we are forced to use shared_ptr over unique_ptr 
    // here (inside of GSCallbackContainer) as the vector stores the base
    // type which does not implement any move semantics

    template <typename T>
    class GSCallbackContainer : public GSCallbackContainerRaw {
    public:

        explicit GSCallbackContainer(std::shared_ptr<Getter<T>> getter, std::shared_ptr<Setter<T>> setter) : mGetter(getter), mSetter(setter) {
        }

        std::shared_ptr<Getter<T>> getter() {
            return mGetter;
        }

        std::shared_ptr<Setter<T>> setter() {
            return mSetter;
        }
    private:
        std::shared_ptr<Getter<T>> mGetter;
        std::shared_ptr<Setter<T>> mSetter;
    };

    std::vector<std::shared_ptr<GSCallbackContainerRaw>> mGSCallbacks;
    std::vector<std::unique_ptr<Callback>> mCallbacks;

    // keep track of fields, buttons and groups for sizing
    std::map<std::string, std::string> mControls; // maps field/button label to group name
    std::map<std::string, bool> mGroups; // maps group name to open/closed status
};

template<>
struct GUIBar::ToTwType <unsigned> {
    static const ETwType value = TW_TYPE_UINT32;
};

template<>
struct GUIBar::ToTwType <bool> {
    static const ETwType value = TW_TYPE_BOOLCPP;
};

template<>
struct GUIBar::ToTwType <float> {
    static const ETwType value = TW_TYPE_FLOAT;
};

template<>
struct GUIBar::ToTwType <double> {
    static const ETwType value = TW_TYPE_DOUBLE;
};

template<>
struct GUIBar::ToTwType <std::string> {
    static const ETwType value = TW_TYPE_STDSTRING;
};
