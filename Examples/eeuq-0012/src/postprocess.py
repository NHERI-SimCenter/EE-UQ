import numpy as np
import xml.etree.ElementTree as ET
import glob
import sys

def main(args):
    # find accel*.xml
    files = glob.glob("Results/accel*.xml")

    found_file = None
    data_array = None

    for file in files:
        tree = ET.parse(file)
        root = tree.getroot()

        data_element = root.find(".//Data")
        if data_element is not None:
            data_text = data_element.text.strip()
            rows = data_text.splitlines()
            split_rows = [row.strip().split() for row in rows if row.strip()]
            if all(len(row) == 4 for row in split_rows):
                data_array = np.array(split_rows, dtype=float)
                found_file = file
                break

    if found_file is None:
        print("No 4-column data found.")
        sys.exit(1)

    # extract columns
    time = data_array[:,0]
    acc_x = data_array[:,1]
    acc_y = data_array[:,2]
    acc_z = data_array[:,3]

    dt = np.diff(time, prepend=time[0])
    g = 9.81

    # integrate for velocity
    vel_x = np.cumsum(acc_x * dt)
    vel_y = np.cumsum(acc_y * dt)
    vel_z = np.cumsum(acc_z * dt)

    # integrate for displacement
    disp_x = np.cumsum(vel_x * dt)
    disp_y = np.cumsum(vel_y * dt)
    disp_z = np.cumsum(vel_z * dt)

    # Arias intensity
    arias_x = np.pi/(2*g) * np.cumsum(acc_x**2 * dt)
    arias_y = np.pi/(2*g) * np.cumsum(acc_y**2 * dt)
    arias_z = np.pi/(2*g) * np.cumsum(acc_z**2 * dt)

    # cumulative absolute velocity
    CAV_x = np.sum(np.abs(acc_x) * dt)
    CAV_y = np.sum(np.abs(acc_y) * dt)
    CAV_z = np.sum(np.abs(acc_z) * dt)

    # RMS
    rms_x = np.sqrt(np.mean(acc_x**2))
    rms_y = np.sqrt(np.mean(acc_y**2))
    rms_z = np.sqrt(np.mean(acc_z**2))

    # Arias final
    arias_total_x = arias_x[-1]
    arias_total_y = arias_y[-1]
    arias_total_z = arias_z[-1]

    # Duration 5-95% Arias
    def significant_duration(arias_curve):
        a5 = 0.05 * arias_curve[-1]
        a95 = 0.95 * arias_curve[-1]
        t5_idx = np.argmax(arias_curve >= a5)
        t95_idx = np.argmax(arias_curve >= a95)
        return time[t95_idx] - time[t5_idx]

    duration_x = significant_duration(arias_x)
    duration_y = significant_duration(arias_y)
    duration_z = significant_duration(arias_z)

    # results
    results = []
    for arg in args:
        if arg == "pga_x":
            val = np.max(np.abs(acc_x))
        elif arg == "pga_y":
            val = np.max(np.abs(acc_y))
        elif arg == "pga_z":
            val = np.max(np.abs(acc_z))
        elif arg == "pgv_x":
            val = np.max(np.abs(vel_x))
        elif arg == "pgv_y":
            val = np.max(np.abs(vel_y))
        elif arg == "pgv_z":
            val = np.max(np.abs(vel_z))
        elif arg == "pgd_x":
            val = np.max(np.abs(disp_x))
        elif arg == "pgd_y":
            val = np.max(np.abs(disp_y))
        elif arg == "pgd_z":
            val = np.max(np.abs(disp_z))
        elif arg == "Arias_x":
            val = arias_total_x
        elif arg == "Arias_y":
            val = arias_total_y
        elif arg == "Arias_z":
            val = arias_total_z
        elif arg == "CAV_x":
            val = CAV_x
        elif arg == "CAV_y":
            val = CAV_y
        elif arg == "CAV_z":
            val = CAV_z
        elif arg == "RMS_x":
            val = rms_x
        elif arg == "RMS_y":
            val = rms_y
        elif arg == "RMS_z":
            val = rms_z
        elif arg == "duration_x":
            val = duration_x
        elif arg == "duration_y":
            val = duration_y
        elif arg == "duration_z":
            val = duration_z
        else:
            print(f"Unknown EDP argument: {arg}")
            val = np.nan
        results.append(val)

    # write results
    with open("results.out","w") as f:
        for r in results:
            f.write(f"{r}\n")

    print(f"Wrote results.out in order: {args}")

if __name__ == "__main__":
    main(sys.argv[1:])
