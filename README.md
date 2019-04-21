# Otus homework 7: bulk

## Описание моей реализации паттерна Наблюдатель:

0) Функциональность основного задания выполнена в полном объеме.<br>
   В случае возникновения ошибки, т.е. если встретится не парная } скобка, программа завершится по исключению

1)	В задании было сказано: использовать паттерн “наблюдатель”, он призван снизить связанность
классов в программе, и желательно именно это и получить. Т.е. низкую связность обработки данных.<br><br>
В связи с этим, я отказался от классической схемы наследования subscribers от абстрактного
интерфейсного класса. От своих подписчиков publisher этого не требует. Единственно, чего требует
publisher от подписчиков, чтобы они имели одну и туже сигнатуру ф-ии уведомления, т.е. один и
тот же тип возвращаемого значения, одинаковое количество и типы аргументов.<br><br>
Таким образом, у меня получилась Обобщенного реализация паттерна Наблюдатель.<br><br>
Сигнатуры подписчиков задается через параметр шаблона класса publisher. Например:
```cpp
     publisher_mixin<void(const std::vector<std::string>&, std::time_t)>
```
задает подписчиков с двумя параметрами<br><br>
Вообще говоря, на сигнатуру подписчиков наложено лишь одно ограничение, количество
аргументов не может быть больше 20. Это связано с количеством placeholders
в используемых реализациях стандартной библиотеки

3)	Таким образом, подписчиками могут быть любые ф-ии и функциональные объекты,
с любым типом возвращаемого значения и аргументов.<br><br>
Например, подписываем функциональные объекты с сигнатурой `void(void)` :
```cpp
void fn(){}

struct subscriber
{
      void test(){};
      void operator()(){}
};

subscriber sscr;

pbl.add_subscriber(fn);                       // подписываем ф-ию
pbl.add_subscriber(sscr);                     // подписываем subscriber::operator()
pbl.add_subscriber(sscr, &subscriber::test);  // подписываем subscriber::test()

```

4) В классе **publisher_mixin** так же добавлена возможность отписаться от уведомления.<br>
Для этого функция добавления подписчика возвращает дескриптор, который затем нужно передать ф-ии удаления подписчика:
```cpp
auto handle = pbl.add_subscriber(subscriber_taged(1), &subscriber_taged::test1);
pbl.del_subscriber(handle);
```
Можно так же отписаться от всех подписчиков:
```cpp
pbl.del_all_subscribers();
```

5)	> **lvalue** функторы сохраняются по ссылке, что позволяет из ф-ии уведомления
    изменять состояние исходного объекта подписчика.<br><br>
    > **rvalue** функторы сохраняются по значению, чтобы они имели возможность после
    подписки дожить до вызова ф-ии уведомления.

6)  Особенности **publisher_mixin**<br>
    Класс *по умолчанию* сохраняет только уникальных подписчиков, т.е. все подписчики получат уведомления
    только один раз.<br><br>
    Для достижения этого пришлось пойти на ряд жертв, в частности, подписчики этого
    класса *по умолчанию* не могут иметь множественное и/или виртуальное наследование.<br><br>
    Но если хочется, то все эти ограничения можно снять.
    Если мы хотим добавить подписчика с множественным и виртуальным наследованием, то делаем так:
```cpp
struct subscriber_md : subscriber_base1, subscriber_base2
{
      void test2(){};
};

publisher pbl;
pbl.add_subscriber(std::bind(&subscriber_md::test2, md));
```
   
   При желании, мы так же можем подписать один и тот же функциональный объект несколько раз.<br>
   Если это обычная ф-ия, то так:    
```cpp
void subscriber_fn(){}

pbl.add_subscriber(std::function<void(void)>(subscriber_fn));
pbl.add_subscriber(std::function<void(void)>(subscriber_fn));
```
   Если это ф-ия член класса, то так:
```cpp
struct subscriber
{
      void test0(){};
};
subscriber sscrb;

pbl.add_subscriber(std::bind(&subscriber::test0, sscrb)); 
pbl.add_subscriber(std::bind(&subscriber::test0, sscrb)); 
```

7)	В целях снижения связности publisher класс сделан в виде mixin класса:  **publisher_mixin**<br>
    Таким образом мы можем добавить его, как базовый класс, в любой другой класс, что автоматически
    добавит функциональность паттерна `наблюдатель` в полученный класс.<br><br>
    Все, что останется сделать в полученном классе, вызвать ф-ию `notify()`, передав ей обновленные данные.

8) Если ф-ия подписчика возвращает значение, то ф-ию `notify()`собирает эти значения и возвращает
   список пар: возвращаемое значение, дескриптор ф-ии - который однозначно задает ф-ию подписчика.<br><br>
   На основании этого дескриптора, мы можем отписать ф-ию подписчика или, вызвав  get_fn_by_handle()
   получить объект function<R(Args...)>, инкапсулирующий ф-ию подписчика.<br>
   Например:
```cpp
template <typename Base = publisher_mixin<int(int, int)>>
struct publisher_not_void : public Base
{
    using Base::notify;
    using Base::get_fn_by_handle;
    using Base::get_last_notify_exception;
       
    using notify_ret_t = std::invoke_result_t<decltype(&publisher_not_void::notify), Base, int, int>;

public:
    notify_ret_t retvs;

    int run()
    {
            int sum = 0;
            retvs = notify(1, 1);

            for (auto& retv : retvs)
            {
                sum += retv.first;                        
            }
            return sum;
    };
};
```

9) В классе **publisher_mixin** так же реализован перехват исключений, которые могут возникнуть у подписчиков
во время уведомления. Все собранные во время последнего уведомления исключения можно получить в классе наследнике от
**publisher_mixin**, через ф-ию:
```cpp
std::list<std::runtime_error> notify_exception_list = get_last_notify_exception();
```
Если список пуст, то при последнем уведомлении подписчики не бросали исключений.

## Некоторые технические детали моей реализации:
В процессе написания кода, я столкнулся с рядом сложностей, и поиск в гугле дал эту статью Herb Sutter
http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/CUJ/2003/0309/cujexp0309sutter/

В этой статье, написанной 2003, Herb Sutter предложил идею реализации обобщенного паттерна Наблюдатель. 
Моя реализация фактически является аналогом реализации Herb Sutter и имеет те же проблемы, что и проблемы, 
с которыми столкнулся Sutter. 

Ключевой проблемой, является ограниченность функциональности std::function. Sutter даже предлагал в статье 
внести изменения в стандарт, чтобы решить эту проблему. Но несмотря на то, что прошло более 10 лет все 
проблемы остались на прежнем месте.

Суть проблемы в том, что сложно создать множество объектов std::function, без дубликатов. Поскольку function 
не имеет операторов сравнения ==, !=, <, >, <=, >=. (Если бы я заранее был бы знаком с этой статьей, я бы не 
взялся за обобщенный подход. Решив, что он не реализуем в принципе в рамках текущего стандарта.)

Для решения этой проблемы, пришлось сделать обвязку вокруг function, т.е. хранить вместе с std::function 
исходные(оригинальные) адреса функциональных объектов и функций. Таким образом вместо std::set использовать 
std::unordered_map, где в качестве ключа и использовать эти адреса. 

std::unordered_map использует для сравнения элементов(ключей) оператор ==. Это позволяет обойти проблему отсутствия 
операторов сравнения у самого std::function. Но порождает другие сложности, связанные с универсальным представлением 
адресов различных объектов в С++

Например, указатель на функцию в общем случае не может быть приведен к типу void* (т.е. указателю на объекты) 
[C++17 8.2.10/8]. Поэтому пришлось хранить указатели на функции и указатели на функциональные объекты, 
как независимые сущности в std::variant. И в этом смысле С++17 все же помогает решить проблему.

Так же пришлось использовать reinterpret_cast для приведения указателей на ф-ии с разными сигнатурами к указателю 
на ф-ию с одной сигнатурой void(*)(void). Аналогично поступить для указателей на функции члены класса. Поскольку 
после преобразования, мы не будем пытаться вызвать ф-ии, а лишь сравниваем числовые значения адресов, то это не 
вызовет неопределенного поведения. Соответствующие комментарии со ссылками на стандарт, допускающими такое 
преобразование и использование, имеются в коде.



Документацию и дополнительное описание проекта можно найти здесь:
https://rra-roro.github.io/homework_7_bulk
