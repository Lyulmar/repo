#include <iostream>
#include <string>
#include <cmath>
#include <cassert>
#include <vector>


struct Transformer;

struct Expression {
    virtual ~Expression() {}
    virtual double evaluate() const = 0;
    virtual Expression* transform(Transformer* tr) const = 0;
};

struct Transformer {
    virtual ~Transformer() {}
    virtual Expression* transformNumber(class Number const*) = 0;
    virtual Expression* transformBinaryOperation(class BinaryOperation const*) = 0;
    virtual Expression* transformFunctionCall(class FunctionCall const*) = 0;
    virtual Expression* transformVariable(class Variable const*) = 0;
};

struct Number : Expression {
    Number(double value) : value_(value) {}
    double value() const { return value_; }
    double evaluate() const override { return value_; }
    Expression* transform(Transformer* tr) const override { return tr->transformNumber(this); }
private:
    double value_;
};

struct BinaryOperation : Expression {
    enum { PLUS = '+', MINUS = '-', DIV = '/', MUL = '*' };

    BinaryOperation(Expression const* left, int op, Expression const* right)
        : left_(left), op_(op), right_(right) {
        assert(left_ && right_);
    }

    ~BinaryOperation() {
        delete left_;
        delete right_;
    }

    double evaluate() const override {
        double l = left_->evaluate();
        double r = right_->evaluate();
        switch (op_) {
        case PLUS: return l + r;
        case MINUS: return l - r;
        case MUL: return l * r;
        case DIV: return l / r;
        default: return 0.0;
        }
    }

    Expression* transform(Transformer* tr) const override {
        return tr->transformBinaryOperation(this);
    }

    Expression const* left() const { return left_; }
    Expression const* right() const { return right_; }
    int operation() const { return op_; }

private:
    Expression const* left_;
    Expression const* right_;
    int op_;
};


struct FunctionCall : Expression {
    FunctionCall(std::string const& name, Expression const* arg)
        : name_(name), arg_(arg) {
        assert(arg_);
        assert(name_ == "sqrt" || name_ == "abs");
    }

    ~FunctionCall() { delete arg_; }

    double evaluate() const override {
        double a = arg_->evaluate();
        if (name_ == "sqrt") return std::sqrt(a);
        else return std::fabs(a);
    }

    Expression* transform(Transformer* tr) const override {
        return tr->transformFunctionCall(this);
    }

    std::string const& name() const { return name_; }
    Expression const* arg() const { return arg_; }

private:
    std::string const name_;
    Expression const* arg_;
};

struct Variable : Expression {
    Variable(std::string const& name) : name_(name) {}

    std::string const& name() const { return name_; }
    double evaluate() const override { return 0.0; }

    Expression* transform(Transformer* tr) const override {
        return tr->transformVariable(this);
    }

private:
    std::string const name_;
};


struct CopySyntaxTree : Transformer {
    Expression* transformNumber(Number const* number) override {
        return new Number(number->value());
    }

    Expression* transformBinaryOperation(BinaryOperation const* binop) override {
        Expression* newLeft = binop->left()->transform(this);
        Expression* newRight = binop->right()->transform(this);
        return new BinaryOperation(newLeft, binop->operation(), newRight);
    }

    Expression* transformFunctionCall(FunctionCall const* fcall) override {
        Expression* newArg = fcall->arg()->transform(this);
        return new FunctionCall(fcall->name(), newArg);
    }

    Expression* transformVariable(Variable const* var) override {
        return new Variable(var->name());
    }
};
struct FoldConstants : Transformer {
    Expression* transformNumber(Number const* number) override {
        return new Number(number->value());
    }

    Expression* transformBinaryOperation(BinaryOperation const* binop) override {
        Expression* newLeft = binop->left()->transform(this);
        Expression* newRight = binop->right()->transform(this);

        Number* leftNum = dynamic_cast<Number*>(newLeft);
        Number* rightNum = dynamic_cast<Number*>(newRight);

        if (leftNum && rightNum) {
            double res;
            switch (binop->operation()) {
            case BinaryOperation::PLUS: res = leftNum->value() + rightNum->value(); break;
            case BinaryOperation::MINUS: res = leftNum->value() - rightNum->value(); break;
            case BinaryOperation::MUL: res = leftNum->value() * rightNum->value(); break;
            case BinaryOperation::DIV: res = leftNum->value() / rightNum->value(); break;
            default: res = 0.0;
            }
            delete newLeft;
            delete newRight;
            return new Number(res);
        }
        return new BinaryOperation(newLeft, binop->operation(), newRight);
    }

    Expression* transformFunctionCall(FunctionCall const* fcall) override {
        Expression* newArg = fcall->arg()->transform(this);
        Number* argNum = dynamic_cast<Number*>(newArg);

        if (argNum) {
            double val = argNum->value();
            double res;
            if (fcall->name() == "sqrt") res = std::sqrt(val);
            else res = std::fabs(val);
            delete newArg;
            return new Number(res);
        }
        return new FunctionCall(fcall->name(), newArg);
    }

    Expression* transformVariable(Variable const* var) override {
        return new Variable(var->name());
    }
};
    struct PrintStrategy {
        virtual ~PrintStrategy() {}
        virtual void print(std::ostream& os, const std::vector<int>& arr) const = 0;
    };

    class RowPrintStrategy : public PrintStrategy {
        size_t elementsPerRow_;
    public:
        explicit RowPrintStrategy(size_t n) : elementsPerRow_(n) {}

        void print(std::ostream& os, const std::vector<int>& arr) const override {
            for (size_t i = 0; i < arr.size(); ++i) {
                os << arr[i];
                if ((i + 1) % elementsPerRow_ == 0)
                    os << '\n';
                else if (i + 1 != arr.size())
                    os << ' ';
            }
            if (arr.size() % elementsPerRow_ != 0)
                os << '\n';
        }
    };

    class LinePrintStrategy : public PrintStrategy {
    public:
        void print(std::ostream& os, const std::vector<int>& arr) const override {
            for (int x : arr)
                os << x << '\n';
        }
    };


    class Array {
        std::vector<int> data_;
        PrintStrategy* strategy_;

    public:
        explicit Array(PrintStrategy* strategy) : strategy_(strategy) {}

        ~Array() {
            delete strategy_;
        }

        void add(int value) {
            data_.push_back(value);
        }

        void setStrategy(PrintStrategy* strategy) {
            delete strategy_;
            strategy_ = strategy;
        }

        void print(std::ostream& os) const {
            strategy_->print(os, data_);
        }

        const std::vector<int>& data() const {
            return data_;
        }
    };

int main() {
    setlocale(LC_ALL, "ru");
    Number* n32 = new Number(32.0);
    Number* n16 = new Number(16.0);
    BinaryOperation* minus = new BinaryOperation(n32, BinaryOperation::MINUS, n16);
    FunctionCall* callSqrt = new FunctionCall("sqrt", minus);
    Variable* var = new Variable("var");
    BinaryOperation* mult = new BinaryOperation(var, BinaryOperation::MUL, callSqrt);
    FunctionCall* original = new FunctionCall("abs", mult);

    CopySyntaxTree cst;
    Expression* copy = original->transform(&cst);

    std::cout << "оригинал: " << original->evaluate() << std::endl;
    std::cout << "копия: " << copy->evaluate() << std::endl;

    FoldConstants fc;
    Expression* folded = original->transform(&fc);

    std::cout << "до: " << original->evaluate() << std::endl;
    std::cout << "после: " << folded->evaluate() << std::endl;

    Array arr(new RowPrintStrategy(3));
    for (int i = 1; i <= 10; i++) {
        arr.add(i);
    }

    std::cout << "3 элемента:" << std::endl;
    arr.print(std::cout);

    std::cout << "1 элементу:" << std::endl;
    Array arr2(new LinePrintStrategy());
    for (int i = 1; i <= 10; i++) {
        arr2.add(i);
    }
    arr2.print(std::cout);

    delete original;
    delete copy;
    delete folded;

    return 0;
}