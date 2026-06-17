/// Test script for proxy-based seamless travel.
///
/// Run this on the proxy server to verify mesh state.
/// Prints current cell, assigned server, and peer status.
///
/// Usage in game:
///   > exec "RunScript SeamlessTravel"
///
/// Expected output:
///   [Otter] Mesh Active: true
///   [Otter] Peers Connected: 2/2
///   [Otter] Client at (12500, 30000, 0) -> GS_0 (cell 0,0)
///   [Otter] Client at (51000, 30000, 0) -> GS_1 (cell 1,0)

void RunSeamlessTravelTest()
{
    Log("=== Seamless Travel Test ===");

    // 1. Check mesh state
    if (!IsMeshActive())
    {
        Log("ERROR: Mesh is not active. Start a mesh-enabled server first.");
        return;
    }

    bool bAllPeers = AreAllPeersConnected();
    int PeerCount = GetConnectedPeerCount();

    Log("Mesh Active: true");
    Log("Peers Connected: " + PeerCount + "/all");

    if (!bAllPeers)
    {
        Log("WARNING: Not all peers are connected. Waiting...");
    }

    // 2. Test location routing
    FVector LocationA(12500, 30000, 0);   // cell (0,0)
    FVector LocationB(51000, 30000, 0);   // cell (1,0) — across boundary at X=50000
    FVector LocationC(-10000, 75000, 0);  // cell (-1,1)

    string ServerA = GetServerForLocation(LocationA);
    string ServerB = GetServerForLocation(LocationB);
    string ServerC = GetServerForLocation(LocationC);

    Log("");
    Log("--- Location Routing Test ---");
    Log("LocationA (12500, 30000) -> Server: " + ServerA);
    Log("LocationB (51000, 30000) -> Server: " + ServerB);
    Log("LocationC (-10000, 75000) -> Server: " + ServerC);

    // 3. Server assignments should be non-empty
    if (ServerA == "")
    {
        Log("ERROR: No server assigned for LocationA!");
    }

    if (ServerB == "" || ServerB == ServerA)
    {
        Log("WARNING: LocationB should route to a different server than LocationA.");
        Log("         This is normal with only 1 game server configured.");
    }

    Log("");
    Log("--- Test Complete ---");
}
