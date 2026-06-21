import csv
import collections
import os

# Mappings from Chadwick/Retrosheet documentation
BASE_LABELS = {
    0: "Empty",
    1: "1st",
    2: "2nd",
    3: "1st & 2nd",
    4: "3rd",
    5: "1st & 3rd",
    6: "2nd & 3rd",
    7: "Loaded"
}

EVENT_LABELS = {
    2: "Generic out",
    3: "Strikeout",
    4: "Stolen base",
    5: "Defensive indifference",
    6: "Caught stealing",
    8: "Pickoff",
    9: "Wild pitch",
    10: "Passed ball",
    11: "Balk",
    12: "Other advance",
    13: "Foul error",
    14: "Walk",
    15: "Intentional walk",
    16: "Hit by pitch",
    17: "Interference",
    18: "Error",
    19: "Fielder's choice",
    20: "Single",
    21: "Double",
    22: "Triple",
    23: "Home run"
}

BATTEDBALL_LABELS = {
    'F': "Fly",
    'G': "Grounder",
    'L': "Liner",
    'P': "Pop-up",
    '': "-"
}

DEST_LABELS = {
    0: "Out",
    1: "1st",
    2: "2nd",
    3: "3rd",
    4: "Score"
}

BATTER_EVENT_CODES = {2, 3, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}

def normalize_dest(d):
    # Map all scoring codes (4, 5, 6, 7) to 4 (Score)
    return 4 if d >= 4 else d

def format_state(outs, bases):
    return f"{outs} out, {BASE_LABELS.get(bases, 'Unknown')}"

def get_runners(base_cd):
    r1 = base_cd in [1, 3, 5, 7]
    r2 = base_cd in [2, 3, 6, 7]
    r3 = base_cd in [4, 5, 6, 7]
    return r1, r2, r3

def get_safe_filename(outs, bases):
    base_name = BASE_LABELS.get(bases, 'Unknown').replace(' & ', '_and_').replace(' ', '_')
    return f"{outs}_out_{base_name}.csv"

def aggregate(filename):
    # transitions[start_state][outcome] = count
    # start_state: (outs, bases)
    # outcome: (event_cd, battedball_cd, bat_dest, run1_dest, run2_dest, run3_dest, end_bases, end_outs, runs)
    transitions = collections.defaultdict(lambda: collections.defaultdict(int))
    total_counts = collections.defaultdict(int)

    with open(filename, 'r') as f:
        reader = csv.reader(f)
        for row in reader:
            if not row or "OUTS_CT" in row[0]: continue
            
            # Index mapping based on cwevent -f 4,34,40,47,48,49,50,58,59,60,61 -x 13,14,45
            start_outs = int(row[0])
            event_cd = int(row[1])
            event_outs = int(row[2])
            battedball_cd = row[3]
            bunt_fl = row[4]
            foul_fl = row[5]
            hit_loc = row[6]
            bat_dest = normalize_dest(int(row[7]))
            run1_dest = normalize_dest(int(row[8]))
            run2_dest = normalize_dest(int(row[9]))
            run3_dest = normalize_dest(int(row[10]))
            start_bases = int(row[11])
            end_bases = int(row[12])
            runs = int(row[13])
            
            end_outs = start_outs + event_outs
            
            start_state = (start_outs, start_bases)
            # Grouping key includes batted ball type, bunt, foul, location and normalized destinations
            outcome = (event_cd, battedball_cd, bunt_fl, foul_fl, hit_loc, bat_dest, run1_dest, run2_dest, run3_dest, end_bases, end_outs, runs)
            
            transitions[start_state][outcome] += 1
            total_counts[start_state] += 1
            
    return transitions, total_counts

def report(transitions, total_counts, output_dir):
    # Sort states by outs then bases
    sorted_states = sorted(total_counts.keys())
    
    for state in sorted_states:
        outs, bases = state
        total = total_counts[state]
        filename = get_safe_filename(outs, bases)
        filepath = os.path.join(output_dir, filename)
        
        with open(filepath, 'w', newline='') as f:
            writer = csv.writer(f)
            
            r1_s, r2_s, r3_s = get_runners(bases)
            headers = ["Play", "Type", "Bnt", "Fl", "Loc", "B"]
            if r1_s: headers.append("R1")
            if r2_s: headers.append("R2")
            if r3_s: headers.append("R3")
            headers.extend(["Count", "Probability", "New Outs", "Runs"])
            
            writer.writerow(headers)
            
            # Sort outcomes by probability descending
            outcomes = transitions[state]
            sorted_outcomes = sorted(outcomes.items(), key=lambda x: x[1], reverse=True)
            
            for outcome, count in sorted_outcomes:
                event_cd, battedball_cd, bunt_fl, foul_fl, hit_loc, bat_dest, run1_dest, run2_dest, run3_dest, end_bases, end_outs, runs = outcome
                prob = (count / total) * 100
                
                event_name = EVENT_LABELS.get(event_cd, f"Event {event_cd}")
                battedball_name = BATTEDBALL_LABELS.get(battedball_cd, battedball_cd)
                bunt_str = "Y" if bunt_fl == "T" else "-"
                foul_str = "Y" if foul_fl == "T" else "-"
                loc_str = hit_loc if hit_loc else "-"
                
                row = [event_name, battedball_name, bunt_str, foul_str, loc_str]
                
                # Batter movement
                if event_cd in BATTER_EVENT_CODES:
                    row.append(DEST_LABELS.get(bat_dest, '?'))
                else:
                    # Non-batter event (Stolen Base, WP, etc.)
                    if bat_dest == 0:
                        row.append("Stays")
                    else:
                        row.append(DEST_LABELS.get(bat_dest, '?'))
                
                # Runner movements
                if r1_s: row.append(DEST_LABELS.get(run1_dest, '?'))
                if r2_s: row.append(DEST_LABELS.get(run2_dest, '?'))
                if r3_s: row.append(DEST_LABELS.get(run3_dest, '?'))
                
                row.append(str(count))
                row.append(f"{prob:6.2f}%")
                row.append("3" if end_outs >= 3 else str(end_outs))
                row.append(str(runs))
                
                writer.writerow(row)

if __name__ == "__main__":
    transitions, total_counts = aggregate("data/raw_transitions_2025.csv")
    output_directory = "data/matrices_2025"
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    report(transitions, total_counts, output_directory)
    print(f"Generated {len(total_counts)} transition matrix files in {output_directory}/")
