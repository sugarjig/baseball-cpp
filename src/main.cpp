#include <iostream>
#include <filesystem>
#include "Simulator.hpp"
#include "StaticEventSource.hpp"
#include "SimulatorObserver.hpp"
#include "Game.hpp"

int main() {
    std::cout << "Starting Chadwick example...\n";

    std::vector<Record> records = {
        {RecordType::Play, PlayInfo{1, 0, "duraj001", "10", "BX", "7/F7D"}},
        {RecordType::Play, PlayInfo{1, 0, "dever001", "32", "BBFSFF.BFB", "W"}},
        {RecordType::Play, PlayInfo{1, 0, "brega001", "10", "B.X", "S7/L78D+.1-2"}},
        {RecordType::Play, PlayInfo{1, 0, "refsr001", "22", "CFBBT", "K"}},
        {RecordType::Play, PlayInfo{1, 0, "stort001", "02", "CFS", "K"}},
        {RecordType::Play, PlayInfo{1, 1, "westj002", "22", "CBFBX", "S8/G6M"}},
        {RecordType::Play, PlayInfo{1, 1, "rutsa001", "11", "CBX", "S7/L7S.1-2"}},
        {RecordType::Play, PlayInfo{1, 1, "oneit001", "22", "CF*BB>X", "S7/G56.2-H;1-2"}},
        {RecordType::Play, PlayInfo{1, 1, "mounr001", "01", "*SX", "S7/L7LS.2-H;1-3(E7);B-2"}},
        {RecordType::Play, PlayInfo{1, 1, "kjerh001", "12", "SBSC", "K"}},
        {RecordType::Play, PlayInfo{1, 1, "laurr001", "12", "BFCS", "K"}},
        {RecordType::Play, PlayInfo{1, 1, "mullc002", "20", "BBX", "D8/L8D.3-H;2-H"}},
        {RecordType::Play, PlayInfo{1, 1, "uriar001", "12", "CFBS", "K"}},
        {RecordType::Play, PlayInfo{2, 0, "campk002", "32", "CBBBCB", "W"}},
        {RecordType::Play, PlayInfo{2, 0, "gonzr003", "01", "S1X", "S7/G56.1-2"}},
        {RecordType::Play, PlayInfo{2, 0, "wongc001", "22", "CBCBC", "K"}},
        {RecordType::Play, PlayInfo{2, 0, "rafac001", "02", "FSX", "53/G5S"}},
        {RecordType::Play, PlayInfo{2, 0, "duraj001", "10", "BX", "T8/F78XD+.2-H;1-H"}},
        {RecordType::Play, PlayInfo{2, 0, "dever001", "22", "FBFBT", "K"}},
        {RecordType::Play, PlayInfo{2, 1, "hollj003", "00", "X", "S8/G6+"}},
        {RecordType::Play, PlayInfo{2, 1, "westj002", "11", "CB>B", "CS2(24)"}},
        {RecordType::Comment, "$Orioles challenged (tag play), call on the field was upheld."},
        {RecordType::Play, PlayInfo{2, 1, "westj002", "31", "CB>B.BB", "W"}},
        {RecordType::Play, PlayInfo{2, 1, "rutsa001", "11", "CBX", "46(1)3/GDP/G4"}},
        {RecordType::Play, PlayInfo{3, 0, "brega001", "32", "CBBFBC", "K"}},
        {RecordType::Play, PlayInfo{3, 0, "refsr001", "12", "BC.SFX", "S7/L78"}},
        {RecordType::Play, PlayInfo{3, 0, "stort001", "22", "FBB1FX", "S9/L9LS.1-3;B-2(E9/TH)"}},
        {RecordType::Play, PlayInfo{3, 0, "campk002", "12", "FFF*BX", "43/G4MD.3-H;2-3"}},
        {RecordType::Play, PlayInfo{3, 0, "gonzr003", "22", "FBBCFF.C", "K"}},
        {RecordType::Play, PlayInfo{3, 1, "oneit001", "01", "CX", "S9/F9LS"}},
        {RecordType::Play, PlayInfo{3, 1, "mounr001", "32", "BBBCSB", "W.1-2"}},
        {RecordType::Play, PlayInfo{3, 1, "kjerh001", "12", "BC*SFC", "K"}},
        {RecordType::Play, PlayInfo{3, 1, "laurr001", "10", "BX", "64(1)3/GDP/G6"}},
        {RecordType::Play, PlayInfo{4, 0, "wongc001", "31", "BBFBX", "13/G15"}},
        {RecordType::Play, PlayInfo{4, 0, "rafac001", "02", "FSX", "53/G5S"}},
        {RecordType::Play, PlayInfo{4, 0, "duraj001", "32", "CBF.BBFX", "43/G34D"}},
        {RecordType::Play, PlayInfo{4, 1, "mullc002", "00", "X", "4/P4D"}},
        {RecordType::Play, PlayInfo{4, 1, "uriar001", "22", "CS.FBBFX", "S67/G56"}},
        {RecordType::Play, PlayInfo{4, 1, "hollj003", "10", "BX", "16(1)3/GDP/G1S"}},
        {RecordType::Play, PlayInfo{5, 0, "dever001", "32", "BFFFBBC", "K"}},
        {RecordType::Play, PlayInfo{5, 0, "brega001", "00", "", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"bowmm001", "Matt Bowman", 1, 0, 1}},
        {RecordType::Play, PlayInfo{5, 0, "brega001", "01", ".CX", "8/F8"}},
        {RecordType::Play, PlayInfo{5, 0, "refsr001", "01", "SX", "S7/G56"}},
        {RecordType::Play, PlayInfo{5, 0, "stort001", "01", "F>B", "SB2"}},
        {RecordType::Play, PlayInfo{5, 0, "stort001", "22", "F>B.F.BS", "K"}},
        {RecordType::Play, PlayInfo{5, 1, "westj002", "00", "", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"whitg002", "Garrett Whitlock", 0, 0, 1}},
        {RecordType::Play, PlayInfo{5, 1, "westj002", "12", ".BCCX", "53/G56"}},
        {RecordType::Play, PlayInfo{5, 1, "rutsa001", "22", "BCBC.*S", "K"}},
        {RecordType::Play, PlayInfo{5, 1, "oneit001", "10", "BX", "D8/L89XD+"}},
        {RecordType::Play, PlayInfo{5, 1, "mounr001", "11", "BCX", "9/F9D"}},
        {RecordType::Play, PlayInfo{6, 0, "campk002", "00", "", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"domis001", "Seranthony Dominguez", 1, 0, 1}},
        {RecordType::Play, PlayInfo{6, 0, "campk002", "22", ".CBS.BC", "K"}},
        {RecordType::Play, PlayInfo{6, 0, "gonzr003", "32", "FBSBFBFX", "9/F89"}},
        {RecordType::Play, PlayInfo{6, 0, "wongc001", "11", "SBX", "4/L4D"}},
        {RecordType::Play, PlayInfo{6, 1, "kjerh001", "11", "CBX", "5/P56D"}},
        {RecordType::Play, PlayInfo{6, 1, "laurr001", "00", "", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"carld002", "Dylan Carlson", 1, 6, 11}},
        {RecordType::Play, PlayInfo{6, 1, "carld002", "22", ".CBC.BFS", "K"}},
        {RecordType::Play, PlayInfo{6, 1, "mullc002", "00", "X", "31/G34"}},
        {RecordType::Play, PlayInfo{7, 0, "rafac001", "00", "", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"carld002", "Dylan Carlson", 1, 6, 9}},
        {RecordType::Play, PlayInfo{7, 0, "rafac001", "00", ".", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"akink001", "Keegan Akin", 1, 0, 1}},
        {RecordType::Play, PlayInfo{7, 0, "rafac001", "01", "..CX", "9/F9LD"}},
        {RecordType::Play, PlayInfo{7, 0, "duraj001", "02", "CS.S", "K"}},
        {RecordType::Play, PlayInfo{7, 0, "dever001", "22", "SFBFBFF.S", "K"}},
        {RecordType::Play, PlayInfo{7, 1, "uriar001", "00", "", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"weisg001", "Greg Weissert", 0, 0, 1}},
        {RecordType::Play, PlayInfo{7, 1, "uriar001", "11", ".BCX", "S7/L7LS"}},
        {RecordType::Play, PlayInfo{7, 1, "hollj003", "32", "S*BBBCC", "K"}},
        {RecordType::Play, PlayInfo{7, 1, "westj002", "01", "CX", "9/L9D"}},
        {RecordType::Play, PlayInfo{7, 1, "rutsa001", "20", "BBX", "9/F9LS"}},
        {RecordType::Play, PlayInfo{8, 0, "brega001", "00", "", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"canoy001", "Yennier Cano", 1, 0, 1}},
        {RecordType::Play, PlayInfo{8, 0, "brega001", "11", ".BCX", "63/G6"}},
        {RecordType::Play, PlayInfo{8, 0, "refsr001", "00", "", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"abrew002", "Wilyer Abreu", 0, 4, 11}},
        {RecordType::Play, PlayInfo{8, 0, "abrew002", "01", ".CX", "13/G4MS"}},
        {RecordType::Play, PlayInfo{8, 0, "stort001", "12", "CS.B*S", "K"}},
        {RecordType::Play, PlayInfo{8, 1, "oneit001", "00", "", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"abrew002", "Wilyer Abreu", 0, 4, 9}},
        {RecordType::Play, PlayInfo{8, 1, "oneit001", "00", ".", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"slatj002", "Justin Slaten", 0, 0, 1}},
        {RecordType::Play, PlayInfo{8, 1, "oneit001", "01", "..FX", "S9/F89"}},
        {RecordType::Play, PlayInfo{8, 1, "mounr001", "00", "X", "S5/G6.1-3(E5/TH);B-2"}},
        {RecordType::Play, PlayInfo{8, 1, "kjerh001", "10", "BX", "S7/L7S.3-H;2-3"}},
        {RecordType::Play, PlayInfo{8, 1, "carld002", "00", "", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"matej003", "Jorge Mateo", 1, 5, 12}},
        {RecordType::Play, PlayInfo{8, 1, "carld002", "20", ".BB>B", "SB2"}},
        {RecordType::Play, PlayInfo{8, 1, "carld002", "30", ".BB>B.*B", "W"}},
        {RecordType::Play, PlayInfo{8, 1, "mullc002", "00", "", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"crisc002", "Cooper Criswell", 0, 0, 1}},
        {RecordType::Play, PlayInfo{8, 1, "mullc002", "10", ".BX", "S18/G4M.3-H;2-H;1-2"}},
        {RecordType::Play, PlayInfo{8, 1, "uriar001", "01", "CX", "54(1)/FO/G6.2-3;B-1"}},
        {RecordType::Comment, "$Umpire reviewed (play at 1st), call on the field was overturned."},
        {RecordType::Play, PlayInfo{8, 1, "hollj003", "00", "X", "S7/L7S.3-H;1-2"}},
        {RecordType::Play, PlayInfo{8, 1, "westj002", "00", "X", "4(1)/FO/G4M.2-3;B-1"}},
        {RecordType::Play, PlayInfo{8, 1, "rutsa001", "21", "B1BFX", "4/L34D"}},
        {RecordType::Play, PlayInfo{9, 0, "campk002", "00", ".", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"bautf001", "Felix Bautista", 1, 0, 1}},
        {RecordType::Play, PlayInfo{9, 0, "campk002", "32", "..BFBS.B*B", "W"}},
        {RecordType::Play, PlayInfo{9, 0, "gonzr003", "00", "B", "WP.1-2"}},
        {RecordType::Play, PlayInfo{9, 0, "gonzr003", "10", "B.X", "D7/L7LD+.2-H"}},
        {RecordType::Play, PlayInfo{9, 0, "wongc001", "00", "X", "63/G6"}},
        {RecordType::Play, PlayInfo{9, 0, "rafac001", "00", "", "NP"}},
        {RecordType::Substitution, SubstitutionInfo{"casat001", "Triston Casas", 0, 9, 11}},
        {RecordType::Play, PlayInfo{9, 0, "casat001", "32", ".BFS.*BFBX", "31/G3.2-3"}},
        {RecordType::Play, PlayInfo{9, 0, "duraj001", "10", "BX", "S7/L78D+.3-H"}},
        {RecordType::Play, PlayInfo{9, 0, "dever001", "10", "B>B", "DI.1-2"}},
        {RecordType::Play, PlayInfo{9, 0, "dever001", "32", "B>B.BCFB", "W"}},
        {RecordType::Play, PlayInfo{9, 0, "brega001", "10", "BX", "5/P15"}}
    };

    StaticEventSource eventSource(std::move(records));
    SimulatorObserver observer;
    Simulator simulator(&eventSource, &observer);

    Game game("BAL202503310", "2025/03/31");
    if (!game) {
        std::cerr << "Failed to create game\n";
        return 1;
    }

    game.AddInfo("hometeam", "BAL");
    game.AddInfo("visteam", "BOS");

    // Starter records
    game.AddStarter({"duraj001", "Jarren Duran", false, 1, 7});
    game.AddStarter({"dever001", "Rafael Devers", false, 2, 10});
    game.AddStarter({"brega001", "Alex Bregman", false, 3, 5});
    game.AddStarter({"refsr001", "Rob Refsnyder", false, 4, 9});
    game.AddStarter({"stort001", "Trevor Story", false, 5, 6});
    game.AddStarter({"campk002", "Kristian Campbell", false, 6, 4});
    game.AddStarter({"gonzr003", "Romy Gonzalez", false, 7, 3});
    game.AddStarter({"wongc001", "Connor Wong", false, 8, 2});
    game.AddStarter({"rafac001", "Ceddanne Rafaela", false, 9, 8});
    game.AddStarter({"newcs001", "Sean Newcomb", false, 0, 1});
    game.AddStarter({"westj002", "Jordan Westburg", true, 1, 4});
    game.AddStarter({"rutsa001", "Adley Rutschman", true, 2, 2});
    game.AddStarter({"oneit001", "Tyler O'Neill", true, 3, 7});
    game.AddStarter({"mounr001", "Ryan Mountcastle", true, 4, 3});
    game.AddStarter({"kjerh001", "Heston Kjerstad", true, 5, 10});
    game.AddStarter({"laurr001", "Ramon Laureano", true, 6, 9});
    game.AddStarter({"mullc002", "Cedric Mullins", true, 7, 8});
    game.AddStarter({"uriar001", "Ramon Urias", true, 8, 5});
    game.AddStarter({"hollj003", "Jackson Holliday", true, 9, 6});
    game.AddStarter({"povic001", "Cade Povich", true, 0, 1});

    simulator.SimulateGame(game);

    if (game.Write("game.evn")) {
        std::cout << "Game written to " << std::filesystem::current_path() << "/game.evn\n";
    } else {
        std::cerr << "Failed to open game.evn for writing\n";
    }

    return 0;
}