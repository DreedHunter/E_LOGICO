// SPDX-License-Identifier: AGPL-3.0-or-later
// Test di null della PDC (spec motore): due rami paralleli, uno con latenza, uno invertito.
// Se la compensazione è corretta la somma è silenzio.
#include "TestHarness.h"

#include "elogico/engine/GraphCompiler.h"
#include "elogico/engine/WorkerPool.h"
#include "elogico/engine/nodes/DelayNode.h"
#include "elogico/engine/nodes/SineNode.h"

#include <algorithm>
#include <memory>
#include <vector>

using namespace elogico;

namespace
{
class PassNode final : public Node
{
public:
    explicit PassNode(float gain = 1.0f) : gain_(gain) {}
    void prepare(double, int) override {}
    void process(const AudioBuffer& io, const ProcessContext&) noexcept override
    {
        for (int c = 0; c < io.numChannels(); ++c)
            for (int i = 0; i < io.numFrames(); ++i)
                io.channel(c)[i] *= gain_;
    }

private:
    float gain_;
};

float renderPeak(CompiledGraph& graph, WorkerPool* pool, int blocks, int block, int skipFrames)
{
    std::vector<float> l(static_cast<std::size_t>(block)), r(static_cast<std::size_t>(block));
    float* outs[] = {l.data(), r.data()};
    const AudioBuffer out(outs, 2, block);
    float peak = 0.0f;
    ProcessContext ctx;
    ctx.numFrames = block;
    for (int b = 0; b < blocks; ++b)
    {
        graph.process(out, ctx, pool);
        if (b * block >= skipFrames)
            peak = std::max(peak, std::max(out.peak(0), out.peak(1)));
        ctx.samplePos += block;
    }
    return peak;
}
} // namespace

TEST_CASE("ordine topologico, livelli e latenza totale")
{
    // sine(1) -> delay 100 (2) -> master(4)
    //         -> pass (3)      -> master(4)
    GraphDescription g;
    g.addNode(1, std::make_shared<SineNode>());
    g.addNode(2, std::make_shared<DelayNode>(100));
    g.addNode(3, std::make_shared<PassNode>());
    g.addNode(4, std::make_shared<PassNode>());
    g.connect(1, 2);
    g.connect(1, 3);
    g.connect(2, 4);
    g.connect(3, 4);
    g.output = 4;

    auto result = GraphCompiler::compile(g, 48000.0, 256);
    CHECK(result.error.empty());
    CHECK(result.graph != nullptr);
    if (!result.graph)
        return;

    const auto& graph = *result.graph;
    CHECK(graph.totalLatency() == 100);
    const auto& master = graph.nodes()[std::size_t(graph.indexOf(4))];
    CHECK(master.level == 2);
    CHECK(master.arrivalLatency == 100);
    int compensated = 0;
    for (const auto& in : master.inputs)
        compensated += in.compensation;
    CHECK(compensated == 100); // solo il ramo senza latenza viene ritardato
}

TEST_CASE("test di null: residuo sotto -140 dB")
{
    for (WorkerPool* pool : {static_cast<WorkerPool*>(nullptr), new WorkerPool(3)})
    {
        GraphDescription g;
        g.addNode(1, std::make_shared<SineNode>(997.0f, 0.5f));
        g.addNode(2, std::make_shared<DelayNode>(317));
        g.addNode(3, std::make_shared<PassNode>(-1.0f)); // invertito, nessuna latenza
        g.addNode(4, std::make_shared<PassNode>());
        g.connect(1, 2);
        g.connect(1, 3);
        g.connect(2, 4);
        g.connect(3, 4);
        g.output = 4;

        auto result = GraphCompiler::compile(g, 48000.0, 128);
        CHECK(result.graph != nullptr);
        if (result.graph)
        {
            const float peak = renderPeak(*result.graph, pool, 200, 128, 1024);
            CHECK(peak < 1e-7f); // -140 dBFS
        }
        delete pool;
    }
}

TEST_CASE("i cicli vengono rifiutati")
{
    GraphDescription g;
    g.addNode(1, std::make_shared<PassNode>());
    g.addNode(2, std::make_shared<PassNode>());
    g.connect(1, 2);
    g.connect(2, 1);
    g.output = 2;
    auto result = GraphCompiler::compile(g, 48000.0, 128);
    CHECK(result.graph == nullptr);
    CHECK(!result.error.empty());
}

TEST_MAIN()
