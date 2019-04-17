# Otus homework 7: bulk

## Особенности моей реализации следующие:

0) Функциональность основного задания выполнена в полном объеме.<br>
   В случае возникновения ошибки, т.е. если встретится не парная } скобка,<br>
   программа завершится по исключению

1)	В задании было сказано: использовать паттерн “наблюдатель”, он призван снизить связанность <br>
классов в программе, и желательно именно это и получить. Т.е. низкую связность обработки данных.<br><br>
В связи с этим, я отказался от классической схемы наследования subscribers от абстрактного <br>
интерфейсного класса. От своих подписчиков publisher этого не требует. Единственно, чего требует <br>
publisher от подписчиков, чтобы они имели одну и туже сигнатуру ф-ии уведомления, т.е. один и <br>
тот же тип возвращаемого значения, одинаковое количество и типы аргументов.  
Сигнатуры подписчиков задается через параметр шаблона класса publisher. Например:
```cpp
     publisher_mixin<void(const std::vector<std::string>&, std::time_t)>
```
задает подписчиков с двумя параметрами

Вообще говоря, на сигнатуру подписчиков наложено лишь одно ограничение, количество <br>
аргументов не может быть больше 20. Это связано с количеством placeholders <br>
в используемых реализациях стандартной библиотеки

3)	Таким образом, подписчиками могут быть любые ф-ии и функциональные объекты, <br>
с любым типом возвращаемого значения и аргументов.<br>
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

4)	>lvalue функторы сохраняются по ссылке, что позволяет из ф-ии уведомления <br>
    изменять состояние исходного объекта подписчика.<br>
    >rvalue функторы сохраняются по значению, чтобы они имели возможность после <br>
    подписки дожить до вызова ф-ии уведомления.

5)	В целях снижения связности publisher класс сделан в виде mixin класса:**publisher_mixin**<br>
    Таким образом мы можем добавить его, как базовый класс, в любой другой класс, что автоматически <br>
    добавит функциональность паттерна `наблюдатель` в полученном классе. Все, что останется сделать <br>
    в полученном классе, вызвать ф-ию `notify()`, передав ей обновленные данные.

6)  Особенности **publisher_mixin**<br>
    Класс *по умолчанию* сохраняет только уникальных подписчиков, т.е. все подписчики получат уведомления<br>
    только один раз.<br><br>
    Для достижения этого пришлось пойти на ряд жертв, в частности, подписчики этого <br>
    класса *по умолчанию* не могут иметь множественное и/или виртуальное наследование.<br><br>
    Но если хочется, то все эти ограничения можно снять.<br>
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
   Если это ф-ия член класса, то так:<br>
```cpp
struct subscriber
{
      void test0(){};
};
subscriber sscrb;

pbl.add_subscriber(std::bind(&subscriber::test0, sscrb)); 
pbl.add_subscriber(std::bind(&subscriber::test0, sscrb)); 
```
7) В классе **publisher_mixin** так же реализован перехват исключений, которые могут возникнуть у подписчиков<br>
во время уведомления. Все собранные во время последнего уведомления исключения можно получить в классе наследнике от 
**publisher_mixin**, через ф-ию:
```cpp
std::list<std::runtime_error> notify_exception_list = get_last_notify_exception();
```
Если список пуст, то при последнем уведомлении подписчики не бросали исключений.

8) В классе **publisher_mixin** так же добавлена возможность отписаться от уведомления.
Для этого функция добавляения подписчика возвращает дескриптор, который затем нужно передать ф-ии удаления подписчика:
```cpp
auto handle = pbl.add_subscriber(subscriber_taged(1), &subscriber_taged::test1);
pbl.del_subscriber(handle);
```
Можно так же отписаться от всех подписчиков:
```cpp
pbl.del_all_subscribers();
```

Документацию и дополнительное описание проекта можно найти здесь:
https://rra-roro.github.io/homework_7_bulk
