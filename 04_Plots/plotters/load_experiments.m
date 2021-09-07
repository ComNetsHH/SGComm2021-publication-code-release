function [experiments] = load_experiments(path_to_processed_results,experiment)



% Locate all files in the target directory
old_dir = pwd;
cd(strcat(path_to_processed_results,"/",experiment,"/"));
all_experiments = dir(pwd);
% Remove . and ..      
all_experiments = all_experiments(3:end);
cd(old_dir);


for experimentID = 1:size(all_experiments,1)
    experiments(experimentID) = load(strcat(all_experiments(experimentID).folder,"/",all_experiments(experimentID).name));
end


end

