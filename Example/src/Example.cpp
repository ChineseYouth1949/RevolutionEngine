#include <Revolution.h>

class Example : public RE::Application {
 public:
  Example() {}
  ~Example() {}
};

RE::Application* RE::CreateApplication() { return new Example(); }