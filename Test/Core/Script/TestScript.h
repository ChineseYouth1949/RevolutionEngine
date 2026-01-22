#include "Base.h"

class A {
 public:
  int value;
  A(int v) : value(v) {}

  void setValue(int v) { value = v; }
  int getValue() const { return value; }
};

TEST(TestScript, TestScript) {
  ScriptManager manager;
  auto& lua = manager.GetState();

  lua.new_usertype<A>("A", sol::constructors<A(int)>(), "value", &A::value, "setValue", &A::setValue, "getValue", &A::getValue);

  auto fileInfo = FileInfo("./Script.lua", false);
  auto loadResult = manager.LoadScript(fileInfo);
  ASSERT_TRUE(loadResult.success);

  auto handle = loadResult.handle;
  auto fun = manager.GetScriptFun(handle);
  ASSERT_TRUE(fun != nullptr);

  int runCount = 5;

  while (runCount-- > 0) {
    A a(0);
    std::cout << "[C++] a : " << a.getValue() << std::endl;

    lua["ptr"] = &a;

    auto res = fun->call();
    if (!res.valid()) {
      std::cout << "Lua run erro" << std::endl;
    } else {
      int lua_result = res;
      std::cout << "[C++] Lua result: " << lua_result << std::endl;
    }

    std::cout << "[C++] a : " << a.getValue() << std::endl << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto reloadRes = manager.ReloadScript(handle);
    ASSERT_TRUE(reloadRes.first);
  }
}