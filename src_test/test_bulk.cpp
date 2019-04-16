#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"

#include "lib_version.h"

#define PRIVATE_TEST 1
#include "publisher_unique.h"

TEST(version, test1)
{
      ASSERT_TRUE(version() > 0);
}


struct publisher : public roro_lib::publisher_unique_mixin<void(void)>
{
      void run()
      {
            notify();
      };
};


void subscriber_fn()
{
}

struct subscriber_functor
{
      void test(){};

      void operator()()
      {
      }
};

namespace roro_lib
{

      class PublisherUniqueMixinTest : public testing::Test
      {
        public:
            void SetUp() {}
            void TearDown() {}
      };

      TEST_F(PublisherUniqueMixinTest, UniqueAddSubscribers1)
      {
            publisher pbl;

            pbl.add_subscriber(subscriber_fn);                  // 1
            ASSERT_TRUE(pbl.subscribers.size() == 1);

            pbl.add_subscriber(subscriber_functor());           // 2
            ASSERT_TRUE(pbl.subscribers.size() == 2);

            subscriber_functor sf;

            pbl.add_subscriber(sf);                             // 3
            ASSERT_TRUE(pbl.subscribers.size() == 3);

            pbl.add_subscriber(sf, &subscriber_functor::test);  // 4
            ASSERT_TRUE(pbl.subscribers.size() == 4);

            // ----
            pbl.add_subscriber(subscriber_functor());           // 5  - новый уникальный функтор
            ASSERT_TRUE(pbl.subscribers.size() == 5);

            // ----
            pbl.add_subscriber(subscriber_fn);                  // Уже есть в подписчиках
            pbl.add_subscriber(sf);                             // Уже есть в подписчиках
            pbl.add_subscriber(sf, &subscriber_functor::test);  // Уже есть в подписчиках

            ASSERT_TRUE(pbl.subscribers.size() == 5);
      }
}

struct subscriber_base1
{
      int m[30];
      void test0(){};
      void test1(){};
      void test2(){};
};

struct subscriber_base2 : subscriber_base1
{
      int m[256];
      void test2(){};
};

struct subscriber_d : subscriber_base2
{
      void test0(){};
};

/*
 Подписчики не могут быть типом с Множественным и виртуальным наследованием
*/


namespace roro_lib
{
      TEST_F(PublisherUniqueMixinTest, UniqueAddSubscribers2)
      {
            publisher pbl;
            subscriber_d sd;

            pbl.add_subscriber(sd, &subscriber_d::test0);
            pbl.add_subscriber(sd, &subscriber_d::test1);
            pbl.add_subscriber(sd, &subscriber_d::test2);

            pbl.add_subscriber(sd, &subscriber_d::subscriber_base1::test0);
            pbl.add_subscriber(sd, &subscriber_d::subscriber_base1::test2);

            pbl.add_subscriber(sd, &subscriber_d::test0);                    // Уже есть в подписчиках
            pbl.add_subscriber(sd, &subscriber_d::subscriber_base1::test2);  // Уже есть в подписчиках

            ASSERT_TRUE(pbl.subscribers.size() == 5);
      }
}

struct i_subscriber
{
      virtual void test0() = 0;
      virtual void test1() = 0;
};

struct subscriber_base : i_subscriber
{
      void test0(){};
      void test1(){};
      virtual void test2(){};
};

struct subscriber_d2 : subscriber_base
{
      void test2(){};
};

namespace roro_lib
{
      TEST_F(PublisherUniqueMixinTest, UniqueAddSubscribers3)
      {
            publisher pbl;

            subscriber_d2 sd1;
            subscriber_d2 sd2;

            
            //i_subscriber& is = sd2;
            subscriber_base& sb = sd2;

            pbl.add_subscriber(sd1, &subscriber_d2::test0);
            pbl.add_subscriber(sd1, &subscriber_d2::test1);
            pbl.add_subscriber(sd1, &subscriber_d2::test2);

            //pbl.add_subscriber(is, &i_subscriber::test0);   // is  - нельзя инстанцировать
            //pbl.add_subscriber(is, &i_subscriber::test1);
            pbl.add_subscriber(sb, &subscriber_d2::test0);
            pbl.add_subscriber(sb, &subscriber_d2::test1);
            //pbl.add_subscriber(sb, &subscriber_d2::test2);  // &subscriber_d2::test2 - обращение к ф-ии что нет в subscriber_base

            ASSERT_TRUE(pbl.subscribers.size() == 5);
      }
}

struct subscriber_state
{
      int st = 0;
      void test() { ++st; };
      void operator()() { ++st; };
};

namespace roro_lib
{
      TEST_F(PublisherUniqueMixinTest, StoreSubscribersByRef)
      {
            publisher pbl;

            subscriber_state sscr;

            pbl.add_subscriber(sscr);
            pbl.add_subscriber(sscr, &subscriber_state::test);
            pbl.run();

            ASSERT_TRUE(sscr.st == 2);
      }

      TEST_F(PublisherUniqueMixinTest, StoreSubscribersByValue)
      {
            publisher pbl;

            subscriber_state sscr;

            pbl.add_subscriber(std::move(sscr));
            pbl.add_subscriber(std::move(sscr), &subscriber_state::test);

            pbl.add_subscriber(subscriber_state());
            pbl.add_subscriber(subscriber_state(), &subscriber_state::test);

            pbl.run();

            ASSERT_TRUE(sscr.st == 0);
      }
}
