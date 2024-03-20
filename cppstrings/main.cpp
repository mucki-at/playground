#include <iostream>
#include <string>
#include <stack>

/**
 * Literal class type that wraps a constant expression string.
 *
 * Uses implicit conversion to allow templates to *seemingly* accept constant strings.
 */
template<size_t N>
struct StringLiteral {
    constexpr StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N, value);
        len = N-1;
    }
    
    char value[N];
    size_t len;
};

template<size_t N>
struct NAry
{};

template<>
struct NAry<0>
{
    std::function<int()> f;
    static constexpr size_t N=0;
};

template<>
struct NAry<1>
{
    std::function<int(int)> f;
    static constexpr size_t N=1;
};


template<typename A, typename B>
static NAry<A::N+B::N> makePlus(A a, B b) { return [a,b]{return a()+b();}; }

static EvalT makeTimes(EvalT a, EvalT b) { return [a,b]{ return a()*b();}; }
static EvalT makeValue(const char v) { int d = v-'0'; return [d]{return d;}; }

static EvalT simplePolish(const char* str, size_t len)
{
    std::stack<EvalT> stack;
    for (int i=0; i<len; ++i)
    {
        if (isspace(str[i]))
        {
            continue;
        }
        else if (isdigit(str[i]))
        {
            stack.push(makeValue(str[i]));
        }
        else if (str[i]=='+')
        {
            if (stack.size()<2) throw std::runtime_error("unbalanced operation + at location "+std::to_string(i));
            auto a = stack.top(); stack.pop();
            auto b = stack.top(); stack.pop();
            stack.push(makePlus(a,b));
        }
        else if (str[i]=='*')
        {
            if (stack.size()<2) throw std::runtime_error("unbalanced operation * at location "+std::to_string(i));
            auto a = stack.top(); stack.pop();
            auto b = stack.top(); stack.pop();
            stack.push(makeTimes(a,b));
        }
        else
        {
            throw std::runtime_error("invalid character at location "+std::to_string(i));
        }
    }
    if (stack.size() != 1) throw std::runtime_error("unbalanced operations at end of input");
    return stack.top();
}

template<StringLiteral value>
EvalT operator ""_eval()
{
    return simplePolish(value.value, value.len);
}

int main(int argc, const char * argv[])
{
    auto f = "35+"_eval;
    std::cout << f() << std::endl;
    return 0;
}
