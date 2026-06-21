import subprocess
import glob
import os

def run_extraction():
    # Fields:
    # 4: OUTS_CT
    # 34: EVENT_CD
    # 40: EVENT_OUTS_CT
    # 47: BATTEDBALL_CD
    # 48: BUNT_FL
    # 49: FOUL_FL
    # 50: BATTEDBALL_LOC_TX
    # 58: BAT_DEST_ID
    # 59: RUN1_DEST_ID
    # 60: RUN2_DEST_ID
    # 61: RUN3_DEST_ID
    # Extended fields:
    # 13: START_BASES_CD
    # 14: END_BASES_CD
    # 45: EVENT_RUNS_CT
    
    fields = "4,34,40,47,48,49,50,58,59,60,61"
    ext_fields = "13,14,45"
    
    header = "OUTS_CT,EVENT_CD,EVENT_OUTS_CT,BATTEDBALL_CD,BUNT_FL,FOUL_FL,BATTEDBALL_LOC_TX,BAT_DEST_ID,RUN1_DEST_ID,RUN2_DEST_ID,RUN3_DEST_ID,START_BASES_CD,END_BASES_CD,EVENT_RUNS_CT"
    
    event_files = glob.glob("tests/integration/fixtures/2025/2025*.EVA") + \
                  glob.glob("tests/integration/fixtures/2025/2025*.EVN")
    if not event_files:
        print("No event files found.")
        return

    print(f"Found {len(event_files)} event files.")
    
    output_file = "data/raw_transitions_2025.csv"
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    
    with open(output_file, "w") as out_f:
        out_f.write(header + "\n")
        
        for event_file in sorted(event_files):
            print(f"Processing {event_file}...")
            # Use -y 2025 so cwevent looks for team2025 in the current directory
            cmd = ["cwevent", "-y", "2025", "-f", fields, "-x", ext_fields, event_file]
            result = subprocess.run(cmd, capture_output=True, text=True, check=True)
            out_f.write(result.stdout)
    
    print(f"Extraction complete. Data saved to {output_file}")

if __name__ == "__main__":
    run_extraction()
