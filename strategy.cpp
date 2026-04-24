#include <iostream>
#include <iomanip>
using namespace std;

struct Print; //предварительное объявление нашей стратегии

class myNumber{ // класс-контекст
    double number;
    int accuracy;
    bool sign;
public:
    Print * strategy;
    myNumber(double number, int accuracy, bool sign, Print* p):
    strategy(p){
        this->number = number;
        this->sign = sign;
        this->accuracy = accuracy;
    }
    void setStrategy(Print* p){strategy = p;}
    void useStrategy(); //реализуем позже, когда напишем стратегии
};

// для семейства алгоритмов напишем иерархию стратегий
struct Print{ // базовая абстрактная стратегия
    virtual void print(double number, int accuracy, bool sign) = 0;
    ~Print(){}
};

//конкретные стратегии
struct PrintAcc: Print {
    void print(double number, int accuracy, bool sign){
        cout<<setprecision(accuracy)<<fixed<<number<<endl;
    }
    ~PrintAcc(){}
};

struct PrintSigned: Print {
    void print(double number, int accuracy, bool sign){
        if (sign && number>0) cout<<"+"<<number<<endl;
        else if (sign && number<0) cout<<number<<endl;
        else cout<<fabs(number)<<endl;
    }
    ~PrintSigned(){}
};

// реализуем использование стратегии в классе-контексте
void myNumber::useStrategy(){
    strategy->print(this->number, this->accuracy, this->sign);
}


int main() {
    
    setlocale(LC_ALL, "russian");
    
    Print *p1 = new PrintAcc();
    Print *p2 = new PrintSigned();
    
    myNumber n1(-1.2345, 3, false, p1); //сначала число с первой стратегией хотим
    n1.useStrategy();
    
    n1.setStrategy(p2);
    n1.useStrategy();
    
    myNumber n2(88.6547356, 17, true, p2);
    n2.useStrategy();
    
    n2.setStrategy(p1);
    n2.useStrategy();

    return 0;
}

/*
-1.234
1.234

+88.655
88.65473559999999509

*/
