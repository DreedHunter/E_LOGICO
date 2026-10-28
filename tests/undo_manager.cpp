// SPDX-License-Identifier: AGPL-3.0-or-later
#include "TestHarness.h"

#include "elogico/model/Commands.h"
#include "elogico/model/EngineGraphBuilder.h"
#include "elogico/model/ProjectDocument.h"

#include <memory>

using namespace elogico;

TEST_CASE("aggiungi traccia, annulla, ripeti con lo stesso id")
{
    ProjectDocument doc;
    int graphChanges = 0;
    doc.addListener([&](const ProjectDocument::Change& c) { graphChanges += c.graphChanged ? 1 : 0; });

    auto cmd = std::make_unique<AddTrackCommand>(TrackType::Audio, "Voce");
    auto* raw = cmd.get();
    doc.perform(std::move(cmd));
    const ObjectId id = raw->trackId();
    CHECK(doc.project().tracks.size() == 1);
    CHECK(doc.undo());
    CHECK(doc.project().tracks.empty());
    CHECK(doc.redo());
    CHECK(doc.project().tracks.size() == 1);
    CHECK(doc.project().tracks[0].id == id);
    CHECK(graphChanges == 3);
}

TEST_CASE("i movimenti del fader diventano un solo passo di undo")
{
    ProjectDocument doc;
    auto add = std::make_unique<AddTrackCommand>(TrackType::Audio, "Basso");
    auto* raw = add.get();
    doc.perform(std::move(add));
    const ObjectId id = raw->trackId();

    doc.undoManager().beginNewTransaction();
    for (float db : {-1.0f, -2.0f, -3.0f, -6.0f})
        doc.perform(std::make_unique<SetTrackVolumeCommand>(id, db));
    CHECK(doc.project().findTrack(id)->volumeDb == -6.0f);
    CHECK(doc.undo());
    CHECK(doc.project().findTrack(id)->volumeDb == 0.0f);
    CHECK(doc.project().tracks.size() == 1); // la traccia è ancora lì
}

TEST_CASE("elimina traccia e ripristino nella stessa posizione")
{
    ProjectDocument doc;
    for (const char* name : {"A", "B", "C"})
        doc.perform(std::make_unique<AddTrackCommand>(TrackType::Audio, name));
    const ObjectId middle = doc.project().tracks[1].id;
    doc.perform(std::make_unique<RemoveTrackCommand>(middle));
    CHECK(doc.project().tracks.size() == 2);
    doc.undo();
    CHECK(doc.project().tracks[1].id == middle);
    CHECK(doc.project().tracks[1].name == "B");
}

TEST_CASE("il grafo del motore ha una strip per traccia più il master")
{
    ProjectDocument doc;
    doc.perform(std::make_unique<AddTrackCommand>(TrackType::Audio, "A"));
    doc.perform(std::make_unique<AddTrackCommand>(TrackType::SoftwareInstrument, "B"));
    EngineGraphBuilder builder;
    const auto graph = builder.build(doc.project());
    CHECK(graph.nodes.size() == 3);
    CHECK(graph.connections.size() == 2);
    CHECK(graph.output == EngineGraphBuilder::kMasterNodeId);
}

TEST_MAIN()
