// #pragma once

// using namespace re::engine::ecs;

// // 模拟具体的 Pass 实现
// class MockPass : public Pass {
//  public:
//   MockPass(PassId id, const wstring& name) : Pass(id, name) {}

//   // 设置依赖的辅助函数
//   void SetBefore(vector<PassId> ids) { m_before = ids; }
//   void SetAfter(vector<PassId> ids) { m_after = ids; }
//   void SetDeps(vector<Dependency> deps) { m_deps = deps; }

//   const vector<PassId>& GetBeforePass() const override { return m_before; }
//   const vector<PassId>& GetAfterPass() const override { return m_after; }
//   const vector<Dependency>& GetDependencies() const override { return m_deps; }

//   void Execute() override {
//     // 模拟执行，打印当前 Pass 名字
//     std::wcout << L"Executing Pass: " << toStd(GetName()) << std::endl;
//   }

//  private:
//   vector<PassId> m_before;
//   vector<PassId> m_after;
//   vector<Dependency> m_deps;
// };

// TEST(SchedulerTest, EnginePipelineSequence) {
//   Scheduler scheduler;

//   auto physics = std::make_shared<MockPass>(101, L"PhysicsUpdate");
//   auto script = std::make_shared<MockPass>(102, L"ScriptUpdate");
//   auto render = std::make_shared<MockPass>(103, L"RenderFrame");

//   // 逻辑依赖：Physics -> Script -> Render
//   script->SetAfter({101});
//   render->SetAfter({102});

//   vector<std::shared_ptr<Pass>> passes = {physics, script, render};

//   scheduler.Compile(passes);
//   scheduler.DumpGraph(L"graphs/pipeline_sequence.dot");
//   scheduler.Execute();
// }

// TEST(SchedulerTest, ResourceConflictAndPriority) {
//   Scheduler scheduler;
//   AccessId gpuResource{1};

//   // 渲染 Pass：高优先级，不可并行 (写入)
//   auto mainRender = std::make_shared<MockPass>(201, L"MainRender");
//   mainRender->SetDeps({{gpuResource, 10, false}});

//   // 拷贝 Pass：低优先级，不可并行
//   auto copyRes = std::make_shared<MockPass>(202, L"CopyResource");
//   copyRes->SetDeps({{gpuResource, 20, false}});

//   // 两个读取 Pass：相同优先级，可以并行
//   auto debugView1 = std::make_shared<MockPass>(203, L"DebugView_UI");
//   debugView1->SetDeps({{gpuResource, 15, true}});

//   auto debugView2 = std::make_shared<MockPass>(204, L"DebugView_Stats");
//   debugView2->SetDeps({{gpuResource, 15, true}});

//   vector<std::shared_ptr<Pass>> passes = {mainRender, copyRes, debugView1, debugView2};

//   scheduler.Compile(passes);
//   scheduler.DumpGraph(L"graphs/resource_dependency.dot");
//   scheduler.Execute();
// }

// TEST(SchedulerTest, FullEngineSimulation) {
//   Scheduler scheduler;
//   AccessId sceneData{100};

//   // 1. 数据准备 (高优先级)
//   auto input = std::make_shared<MockPass>(1, L"InputSystem");
//   input->SetDeps({{sceneData, 0, false}});

//   // 2. 并行系统：AI 和 音频 (同优先级，可并行)
//   auto ai = std::make_shared<MockPass>(2, L"AISystem");
//   ai->SetDeps({{sceneData, 1, true}});

//   auto audio = std::make_shared<MockPass>(3, L"AudioSystem");
//   audio->SetDeps({{sceneData, 1, true}});

//   // 3. 渲染 (依赖 AI 结果，显式依赖)
//   auto render = std::make_shared<MockPass>(4, L"RenderSystem");
//   render->SetAfter({2});

//   vector<std::shared_ptr<Pass>> passes = {input, ai, audio, render};

//   scheduler.Compile(passes);
//   scheduler.DumpGraph(L"graphs/full_engine.dot");
//   scheduler.Execute();
// }