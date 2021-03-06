clearvars -except subset;
close all;
rng(1);
%% MCDS Processing
% This script setups mcds processing, loads specified feeder data, runs
% mcds routines and saves the resulting graph info individual mcds info
% structs
% Please check the configuration section and adjust variables according to
% your system.

%% Configuration
disp("### Configuration MCDS Processing: ...")

pp_linux = "/simresults/sim/USER/sg-flooding/";
pp_win = "C:\Users\USER\Documents\";
path_prefix = pp_linux; % Select platform prefix

% Path to save_node information generated by osm_processing
path_to_input = strcat(path_prefix, "XX_output_osm");

% Decide for LP or exhaustive search solver
use_lp = true;
% If LP solver is used, please specify output dir for problems
path_export_lp_problem = strcat(path_prefix, "XX_output_lp_problems");

% Process only subset of all generated feeder
if(~exist('subset','var'))
    subset = [31];
end

% Path to export feeder info structs
path_export_feeder_info = strcat(path_prefix, "XX_output_mcds");

% Unit disk communication range in meters
start_communication_range = 100;
communication_range_increments = 1;

% Create export path
mkdir(path_export_feeder_info)
mkdir(path_export_lp_problem)

disp("### Configuration MCDS Processing: Done!")

%% Calculation
for osmDataID = 1:length(subset)    
    % Load Data
    disp(strcat("### Loading node data for ID: ",num2str(subset(osmDataID)), ": ..."));
    load(strcat(path_to_input,"/feeder",num2str(subset(osmDataID)),".mat"))
    disp(strcat("### Loading node data for ID: ",num2str(subset(osmDataID)), ": Done!"));

    % Find MCDS
    disp("### Calculating MCDS: ...");
    export_str = strcat(path_export_lp_problem,"/mcds",num2str(subset(osmDataID)));
    [full_graph, mcds_graph, min_com_range] = get_mcds(save_nodes,start_communication_range,communication_range_increments,use_lp,export_str);
    disp(strcat("### MCDS found with communication range of ", num2str(min_com_range), " meter."));
    disp("### Calculating MCDS: Done!");
    
    if(~use_lp)
        % Plot Results
        disp("### Drawing result figure: ...");
        H = plot_mcds_results(full_graph,mcds_graph);
        drawnow();
        disp("### Drawing result figure: Done!");

        % Save Results
        disp("### Saving full and MCDS graph: ...");
        save_mcds_info(full_graph, mcds_graph, path_export_feeder_info,subset(osmDataID),min_com_range);
        save(strcat(path_export_feeder_info,"/mcdsWS",num2str(subset(osmDataID)),".mat"))
        disp("### Saving full and MCDS graph: Done!");
    end
end
