#!/bin/sh

RESULTS_DIR="results"

EXPERIMENTS="
    experiment_00a_baseline
    experiment_00b_baseline
    experiment_01a_multithreading_baseline
    experiment_01b_multithreading_baseline
    experiment_02a_scalar_madness
    experiment_02b_scalar_madness
    experiment_03_access_pattern
    experiment_04a_data_amount
    experiment_05_dimes_1
    experiment_05paper
    experiment_06_element_size
"

for exp in $EXPERIMENTS; do
    DIR="$RESULTS_DIR/$exp"

    if [ -d "$DIR" ]; then
        if find "$DIR" -maxdepth 1 -name '*.csv' | grep -q .; then

            case "$exp" in
                experiment_00a_baseline)
                    docker run -v "$PWD":/visualize/ -it --rm gather_investigation_plotter Rscript script/experiment_00_visualize0.R
                    ;;
                experiment_00b_baseline)
                    docker run -v "$PWD":/visualize/ -it --rm gather_investigation_plotter Rscript script/experiment_00_visualize1.R
                    ;;
                experiment_01a_multithreading_baseline)
                    docker run -v "$PWD":/visualize/ -it --rm gather_investigation_plotter Rscript script/experiment_01_visualize0.R
                    ;;
                experiment_01b_multithreading_baseline)
                    docker run -v "$PWD":/visualize/ -it --rm gather_investigation_plotter Rscript script/experiment_01_visualize1.R
                    ;;
                experiment_02a_scalar_madness)
                    docker run -v "$PWD":/visualize/ -it --rm gather_investigation_plotter Rscript script/experiment_02_visualize0.R
                    ;;
                experiment_02b_scalar_madness)
                    docker run -v "$PWD":/visualize/ -it --rm gather_investigation_plotter Rscript script/experiment_02_visualize2.R
                    ;;
                experiment_03_access_pattern)
                    docker run -v "$PWD":/visualize/ -it --rm gather_investigation_plotter Rscript script/experiment_03_visualize.R
                    ;;
                experiment_04a_data_amount)
                    docker run -v "$PWD":/visualize/ -it --rm gather_investigation_plotter Rscript script/experiment_04_visualize.R
                    ;;
                experiment_05_dimes_1)
                    docker run -v "$PWD":/visualize/ -it --rm gather_investigation_plotter Rscript script/experiment_05_visualize.R
                    ;;
                experiment_05paper)
                    docker run -v "$PWD":/visualize/ -it --rm gather_investigation_plotter Rscript script/experiment_05_visualize_p.R
                    ;;
                    
                experiment_06_element_size)
                    docker run -v "$PWD":/visualize/ -it --rm gather_investigation_plotter Rscript script/experiment_06_visualize.R
                    ;;
            esac
        else
            echo "No csv file found in $DIR. Skipping..."
        fi
    else
        echo "$DIR does not exist. Skipping..."
    fi
done