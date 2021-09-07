clear all;
close all;
rng(1);

%% OSM Processing
% This script setups osm processing, loads osm data, runs osm_processing 
% routines and saves all resulting VL feeder topologies in individual
% feeder info structs
% Please check the configuration section and adjust variables according to
% your system.

%% Configuration
disp("### Configuration OSM Processing: ...")

pp_linux = "/simresults/sim/USER/sg-flooding/";
pp_win = "C:\Users\USER\Documents\";
path_prefix = pp_linux; % Select platform prefix

% Path to export feeder info structs
path_export_feeder_info = strcat(path_prefix, "XX_output_osm");

% Path to data set
path_to_osm_data = strcat(path_prefix, "osm_map_extract.osm");

% Distance for which adjacent nodes are considered connected:
street_fix_distance = 0.001; 

% Target feeder characteristics
mean_num_loads = 87; % 101 urban, 87 sub-urban, 51 rural

% kmedoids configuration 
km_opts = statset('Display','iter','UseParallel',true,'MaxIter',100);
km_num_replicates = 16;

% Create export path
mkdir(path_export_feeder_info)

disp("### Configuration OSM Processing: Done!")

%% Calculation
% OSM - Processing
disp("### OSM Data Parsing: ...")
[fullGraph, buildingInfo, streetInfo] = parse_osm_data(path_to_osm_data, street_fix_distance);
disp("### OSM Data Parsing: Done!")

% Intermediate processing save point
disp("### OSM Data Save: ...")
save(strcat(path_export_feeder_info,"/osmData.mat"),"fullGraph", "buildingInfo", "streetInfo");
disp("### OSM Data Save: Done!")

% Create LV Feeder Data
disp("### K-Medoids LV Feeder Generation: ...")
[feederMapping] = create_kmedoid_feeder(mean_num_loads, km_num_replicates, km_opts, fullGraph, buildingInfo);
disp("### K-Medoids LV Feeder Generation: Done!")


%% Clean Up
% Export processing save point
disp("### OSM Data Save: ...")
save_feeder_info(fullGraph,feederMapping,path_export_feeder_info);
disp("### OSM Data Save: Done!")

% Plotting debug figure
disp("### Plotting results: ...")
plot_osm_results(fullGraph,feederMapping);
disp("### Plotting results: Done!")

% Plotting statistic figure
disp("### Plotting statistics: ...")
plot_osm_statistics(fullGraph,feederMapping,false);
disp("### Plotting statistics: Done!")

% Save Workspace
save(strcat(path_export_feeder_info,"/osmWS.mat"))
