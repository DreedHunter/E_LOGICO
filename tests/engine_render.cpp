// SPDX-License-Identifier: AGPL-3.0-or-later
#include "TestHarness.h"

#include "elogico/engine/Engine.h"
#include "elogico/engine/nodes/ChannelStripNode.h"
#include "elogico/engine/nodes/SineNode.h"

#include <algorithm>
#include <vector>

using namespace elogico;

namespace
{
std::vector<float> render(Engine& engine, int blocks, int block)
{
    std::vector<float> l(static_cast<std::size_t>(block)), r(static_cast<std::size_t>(block)), all;
    float* outs[] = {l.data(), r.data()};
    for (int b = 0; b < blocks; ++b)
    {
        engine.processOffline(outs, 2, block);
        all.insert(all.end(), l.begin(), l.end());
    }
    return all;
}

GraphDescription graphWith(std::shared_ptr<ChannelStripNode> strip)
{
    GraphDescription g;
    g.addNode(1, strip);
    g.addNode(2, std::make_shared<SineNode>(440.0f, 0.5f));
    g.connect(2, 1);
    g.output = 1;
    return g;
}
} // namespace

TEST_CASE("senza grafo esce silenzio")
{
    Engine engine(0);
    engine.audioDeviceAboutToStart(48000.0, 256, 0, 2);
    const auto out = render(engine, 4, 256);
    float peak = 0.0f;
    for (float s : out)
        peak = std::max(peak, std::fabs(s));
    CHECK(peak == 0.0f);
}

TEST_CASE("render deterministico: due motori, stessi campioni")
{
    std::vector<float> a, b;
    for (auto* target : {&a, &b})
    {
        Engine engine(2);
        engine.audioDeviceAboutToStart(48000.0, 256, 0, 2);
        CHECK(engine.setGraph(graphWith(std::make_shared<ChannelStripNode>())).empty());
        *target = render(engine, 50, 256);
    }
    CHECK(a == b);
}

TEST_CASE("mute porta il canale a zero dopo la rampa")
{
    Engine engine(0);
    engine.audioDeviceAboutToStart(48000.0, 256, 0, 2);
    auto strip = std::make_shared<ChannelStripNode>();
    CHECK(engine.setGraph(graphWith(strip)).empty());
    render(engine, 4, 256);
    strip->setMuted(true);
    render(engine, 8, 256); // rampa di 20 ms = 960 campioni
    CHECK(strip->peak(0) == 0.0f);
}

TEST_CASE("trasporto: play e locate")
{
    Engine engine(0);
    engine.audioDeviceAboutToStart(48000.0, 256, 0, 2);
    engine.play();
    render(engine, 10, 256);
    CHECK(engine.feedback().position.load() == 2560);
    engine.locate(0);
    engine.stop();
    render(engine, 1, 256);
    CHECK(engine.feedback().position.load() == 0);
    CHECK(!engine.feedback().playing.load());
}

TEST_MAIN()
