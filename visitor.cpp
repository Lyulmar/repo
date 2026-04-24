#include <iostream>
#include <vector>
using namespace std;

class Visitor; //предварительное описание

//  Схема классов-элементов, менять ее нельзя, но новую функциональность очень надо
class Lesson { //абстрактный класс-элемент
public:
    virtual void Accept(Visitor *v)=0;
    virtual ~Lesson(){}
};

// конкретные элементы
class Lection: public Lesson {
public:
    void Accept(Visitor *v); //реализацию опишем позже, когда опишем визиторов
    ~Lection(){}
};

class Lab: public Lesson {
public:
    void Accept(Visitor *v); //реализация позже
    ~Lab(){}
};

// Заведем иерархию посетителей для добавления новой функциональности
class Visitor { //абстрактный посетитель
public:
    virtual void VisitLection(Lesson *l) = 0;
    virtual void VisitLab(Lesson *l) = 0;
    virtual ~Visitor(){}
};

// конкретные посетители
// 1 посетитель = 1 новая функция к каждому конкретному классу-элементу
class Teacher: public Visitor {
public:
    void VisitLection(Lesson *l) {cout<<"Преподаватель читает лекцию\n";}
    void VisitLab(Lesson *l) {cout<<"Преподаватель принимает лабораторную\n";}
    ~Teacher(){}
};

class Student: public Visitor {
public:
    void VisitLection(Lesson *l) {cout<<"Студент внимательно конспектирует лекцию\n";}
    void VisitLab(Lesson *l){cout<<"Студент сдает лабораторную работу\n";}
    ~Student(){}
    
};

class LazyStudent: public Visitor {
public:
    void VisitLection(Lesson * l){cout<<"Студент-прогульщик не пошел на лекцию\n";}
    void VisitLab(Lesson *l){cout<<"Студент-прогульщик списал лабораторную\n";}
    ~LazyStudent(){}
};

// напишем теперь реализацию методов Accept у классов-элементов
void Lection::Accept(Visitor* v){v->VisitLection(this);}
void Lab::Accept(Visitor* v){v->VisitLab(this);}

int main() {
    
    setlocale(LC_ALL, "russian");
    
    vector<Lesson*> Tuesday = {new Lection(), new Lab(), new Lab()}; //структура данных с объектами элементов
    
    Visitor *pr = new Teacher();
    Visitor *st1 = new Student();
    Visitor *st2 = new Student();
    Visitor *st3 = new LazyStudent();
    
    vector<Visitor*> v = {pr, st1, st2, st3};
    
    for (int i=0; i<Tuesday.size(); i++){
        for (int j=0; j<v.size(); j++)
            Tuesday[i]->Accept(v[j]);
        cout<<endl;
    }

     
    return 0;
}
/*
Преподаватель читает лекцию
Студент внимательно конспектирует лекцию
Студент внимательно конспектирует лекцию
Студент-прогульщик не пошел на лекцию

Преподаватель принимает лабораторную
Студент сдает лабораторную работу
Студент сдает лабораторную работу
Студент-прогульщик списал лабораторную

Преподаватель принимает лабораторную
Студент сдает лабораторную работу
Студент сдает лабораторную работу
Студент-прогульщик списал лабораторную

*/