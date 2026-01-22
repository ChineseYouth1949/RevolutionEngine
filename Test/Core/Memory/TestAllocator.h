#include "Base.h"

#include <string>
#include <vector>

static bool sPersonDestroy = false;

struct Person {
  Person(std::string _name, int _info) : name(_name), info(_info) {}
  ~Person() { sPersonDestroy = true; }

  std::string name;
  int info;
};

TEST(TestAllocator, MIMALLOC) {
  CLASS_ALLOCATOR(AllocType::MiMalloc);

  // Alloc
  auto person = Allocator::Alloc<Person>();
  ASSERT_TRUE(CheckPtrMemoryValid(person, sizeof(Person)));

  // AllocZero
  person = Allocator::AllocZero<Person>();
  ASSERT_TRUE(CheckPtrMemoryZero(person, sizeof(Person)));

  person->name = std::string("Jak");
  ASSERT_EQ(person->name, std::string("Jak"));

  person->info = 10;
  ASSERT_EQ(person->info, 10);

  // Create
  person = Allocator::Create<Person>(std::string("Bob"), 20);
  ASSERT_EQ(person->name, std::string("Bob"));
  ASSERT_EQ(person->info, 20);

  // Destroy
  Allocator::Destroy(person);
  ASSERT_EQ(sPersonDestroy, true);

  // CreateSharedPtr
  Allocator::SharedPtr sPerson = Allocator::CreateSharedPtr<Person>(std::string("Bob"), 20);
  auto sPerson2 = sPerson;

  sPersonDestroy = false;
  sPerson = nullptr;
  ASSERT_EQ(sPersonDestroy, false);
  ASSERT_EQ(sPerson2->info, 20);

  sPersonDestroy = false;
  sPerson2 = nullptr;
  ASSERT_EQ(sPersonDestroy, true);

  // CreateUniquePtr
  Allocator::UniquePtr uPerson = Allocator::CreateUniquePtr<Person>(std::string("Bob"), 20);
  sPersonDestroy = false;
  uPerson = nullptr;
  ASSERT_EQ(sPersonDestroy, true);

  // StdAllocator-STL
  {
    std::vector<Person, StdAllocator<AllocType::MiMalloc, Person>> personArray;
    for (size_t i = 0; i < 10; i++) {
      auto& newPerson = personArray.emplace_back(std::string("Bob"), 20);
      newPerson.name = std::string("Jak") + std::to_string(i);
      newPerson.info = i;
    }
    ASSERT_EQ(personArray.size(), 10);
  }

  // StdAllocator-MIMALLOC
  {
    std::vector<Person, StdAllocator<AllocType::MiMalloc, Person>> personArray;
    for (size_t i = 0; i < 10; i++) {
      auto& newPerson = personArray.emplace_back(std::string("Bob"), 20);
      newPerson.name = std::string("Jak") + std::to_string(i);
      newPerson.info = i;
    }
    ASSERT_EQ(personArray.size(), 10);
  }
}