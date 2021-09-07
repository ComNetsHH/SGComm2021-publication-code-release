function [] = d_result_processing(path_to_sim_results,path_export_processed,experiment_name,calculate_peak_aoi)
    rng(1);

    info = split(experiment_name,'-');

    % Create export path
    mkdir(strcat(path_export_processed,"/",info(1)))

    disp("### Configuration Simulation Result Processing: Done!")

    %% Calculation
    disp("### Searching for simulation files: ...");
        experiment = gen_new_experiments(path_to_sim_results,experiment_name);
    disp("### Searching for all matching result files: Done!");
    disp(strcat("### Processing experiment : ..."));  
        tic;
        result = process_experiment(path_to_sim_results,experiment,calculate_peak_aoi);
    disp(strcat("### Processing experiment : Done! in ", num2str(toc), " seconds."));
    disp(strcat("### Saving results: ..."));  
        save_results(strcat(path_export_processed,"/",info(1),"/",experiment.identifier,"-",experiment.rngID,".mat"),result)
    disp(strcat("### Saving results: Done!"));

end