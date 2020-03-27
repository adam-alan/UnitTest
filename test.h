#pragma once

#include <vector>
#include <iostream>
#include <functional>
#include <map>

#define registerTest(functionName) register_test_func(std::bind(&functionName, this), #functionName)

#define assertSomething(thing, ...) do {\
    prepare_test(__FILE__, __func__, __LINE__, "assert"#thing, #__VA_ARGS__); \
    _assert##thing(__VA_ARGS__); \
} while(0)

#define assertEqual(...) assertSomething(Equal, __VA_ARGS__)

class Test {
private:
    const char *file;
    const char *func;
    const char *assertName;
    int line;
    std::vector<std::string> args;
    const char *args_s;
    std::map<std::string, std::function<void(void)>> func_map;

    bool isid(int x) {
        return isalnum(x) || x == '_';
    }

    void make_args_array() {
        int i = 0;
        int j = 0;
        this->args.clear();
        while (isspace(args_s[i]))
            i++;
        j = i;
        while (true) {
            if (args_s[j] != ',' && args_s[j] != 0) {
                j++;
            } else {
                if (i < j) {
                    args.emplace_back(&args_s[i], j - i);
                }
                if (args_s[j] == '\0')
                    break;
                j++;
                while (isspace(args_s[j]))
                    j++;
                i = j;
            }
        }
    }

    void fail(std::function<void(void)>fn) {
        using namespace std;
        cerr << endl << "TEST FAIL AT " << func << ":" << line << ":[ " << assertName << "(" << args_s << ") ]"<< endl;
        fn();
        exit(-1);
    }

protected:
    void register_test_func(
            const std::function<void(void)>& f,
            const std::string& name) {

        func_map.emplace(name, f);
    }

    void prepare_test(const char *file,
            const char *func,
            int line,
            const char *assertName,
            const char *args) {
        this->file = file;
        this->func = func;
        this->line = line;
        this->assertName = assertName;
        this->args_s = args;
        make_args_array();
    }

    template <typename T> void _assertEqual(T a, T b) {
        if (a != b) {
            fail([&](){
                using namespace std;
                cerr << args[0] << " expects " << b << " but gets " << a << endl;
            });
        }
    }

public:
    void testAll() {
        using namespace std;
        for (const auto& k : func_map) {
            cerr << "Running " << k.first << "...";
            k.second();
            cerr << "Pass!" << endl;
        }
        cerr << "All test passed! OvO" << endl;
    }
};

