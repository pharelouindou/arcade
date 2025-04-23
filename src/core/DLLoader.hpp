#ifndef DLLOADER_HPP
#define DLLOADER_HPP

#include <string>
#include <stdexcept>
#include <memory>
#include <dlfcn.h>

namespace arcade {

template <typename T>
class DLLoader {
public:
    DLLoader(const std::string &path) {
        _handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!_handle) {
            const char* error = dlerror();
            throw std::runtime_error("dlopen failed: " + std::string(error ? error : "Unknown error"));
        }
        _path = path;
    }

    ~DLLoader() {
        if (_instance) {
            auto destroyer = getSymbol<destroyer_t>("destroy");
            if (destroyer) {
                destroyer(_instance.get());
            }
        }
        if (_handle) {
            dlclose(_handle);
        }
    }

    std::shared_ptr<T> getInstance() {
        if (!_instance) {
            dlerror();
            auto creator = getSymbol<creator_t>("create");
            if (!creator) {
                throw std::runtime_error("Cannot find create symbol: " + _path + ": " + std::string(dlerror()));
            }
            T* raw_instance = creator();
            if (!raw_instance) {
                throw std::runtime_error("Creator returned nullptr");
            }
            auto destroyer = getSymbol<destroyer_t>("destroy");
            _instance = std::shared_ptr<T>(raw_instance, [destroyer](T* ptr) {
                if (destroyer) destroyer(ptr);
            });
        }
        return _instance;
    }

    std::string getPath() const { return _path; }

private:
    template<typename Func>
    Func getSymbol(const std::string& name) {
        dlerror();
        void* symbol = dlsym(_handle, name.c_str());
        const char* error = dlerror();
        if (error) {
            throw std::runtime_error("Cannot find " + name + " symbol: " + _path + ": " + error);
        }
        return reinterpret_cast<Func>(symbol);
    }

    void* _handle;
    std::string _path;
    std::shared_ptr<T> _instance;
    using creator_t = T* (*)();
    using destroyer_t = void (*)(T*);
};

extern template class DLLoader<class IGraphical>;
extern template class DLLoader<class IGame>;

}

#endif