function [new_experiment] = gen_new_experiments(path_export_sim_results,experiment_name)

    old_dir = pwd;
    cd(path_export_sim_results)
    all_experiments = dir(pwd);
    
    %% Remove . and ..
    all_experiments = all_experiments(3:end);

    % Check for matching experiments and cache
    for experiment_id = 1:length(all_experiments)
        if(strcmp(all_experiments(experiment_id).name,experiment_name))
            exp_info = split(all_experiments(experiment_id).name,'-');  
            identifier = strcat(exp_info{1},"-",exp_info{2},"-",exp_info{3},"-",exp_info{4});
            
            new_experiment.identifier = identifier;
            new_experiment.files = [{all_experiments(experiment_id).name}];
            new_experiment.numSeq = str2double(exp_info{3});
            new_experiment.llTech = exp_info{2};
            new_experiment.name = exp_info{1};
            new_experiment.rngID = exp_info{6};
            new_experiment.dynamicRTX = exp_info{5};
            new_experiment.rtxTimer = str2double(exp_info{4});
            break;
        end
    end
    cd(old_dir);
end

