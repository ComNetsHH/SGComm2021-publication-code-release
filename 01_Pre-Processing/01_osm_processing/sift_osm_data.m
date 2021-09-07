function [buildings,streets] = sift_osm_data(raw_data)
    % Earth Radius
    R = 6371;

    %% Ectract Node Info
    nodes = [raw_data.osm.node(:)];
    for i = 1:length(nodes)
        nodeInfo(i) = nodes(i);
        temp = nodeInfo{i};
        nodeIDs(i) = string(temp.Attributes.id);
    end

    %% Find all Ways which are either Buildings or Highways
    streetIDs = [];
    buildingIDs = [];
    streetTypes = ["highway","access"];
    buildingTypes = ["addr:street","building"];

    for wayID = 1:length(raw_data.osm.way)
        if(isfield(raw_data.osm.way{wayID},'tag'))
            % Check for street
            if(length(raw_data.osm.way{wayID}.tag) > 1)
                temp = [raw_data.osm.way{wayID}.tag{:}];
                temp2 = [temp.Attributes];
                if(contains([temp2.k],streetTypes))
                    streetIDs(end+1) = wayID;
                end
            end
            % Check for Building
            if(length(raw_data.osm.way{wayID}.tag) > 1)
                temp = [raw_data.osm.way{wayID}.tag{:}];
                temp2 = [temp.Attributes];
                if(contains([temp2.k],buildingTypes))
                    buildingIDs(end+1) = wayID;
                end
            end
        end
    end

    %%
    % Extract all data for streets
    streetEntryID = 1;
    for index_wayID = 1:length(streetIDs)
       street = raw_data.osm.way{streetIDs(index_wayID)};
       clear nodeID latCoords lonCoords
       for ndi = 2:length(street.nd)
           nodeID1 = find(nodeIDs == string(street.nd{ndi-1}.Attributes.ref));
           nodeID2 = find(nodeIDs == string(street.nd{ndi}.Attributes.ref));

           latCoords1 = str2num(string(nodeInfo{nodeID1}.Attributes.lat));
           latCoords2 = str2num(string(nodeInfo{nodeID2}.Attributes.lat));

           lonCoords1 = str2num(string(nodeInfo{nodeID1}.Attributes.lon));
           lonCoords2 = str2num(string(nodeInfo{nodeID2}.Attributes.lon));

           streets(streetEntryID).latCoords = [latCoords1,latCoords2];
           streets(streetEntryID).lonCoords = [lonCoords1,lonCoords2];

           [y,x,z] = sph2cart(deg2rad([lonCoords1,lonCoords2]),deg2rad([latCoords1,latCoords2]), R);

           streets(streetEntryID).x = x;
           streets(streetEntryID).y = y;
           streets(streetEntryID).w = sqrt((x(1)-x(2))^2+(y(1)-y(2))^2);
           streetEntryID = streetEntryID+1;
          end
       % Find all coords
    end

    % Extract all data for buildings
    for index_buildID = 1:length(buildingIDs)
       building = raw_data.osm.way{buildingIDs(index_buildID)};
       nodeID = [];
       clear nodeID latCoords lonCoords
       for ndi = 1:length(building.nd)
            nodeID(ndi) = find(nodeIDs == string(building.nd{ndi}.Attributes.ref));
            latCoords(ndi) = str2num(string(nodeInfo{nodeID(ndi)}.Attributes.lat));
            lonCoords(ndi) = str2num(string(nodeInfo{nodeID(ndi)}.Attributes.lon));
       end
       buildings(index_buildID).numPoly = ndi;
       buildings(index_buildID).latCoords = latCoords;
       buildings(index_buildID).lonCoords = lonCoords;
       [y,x,z] = sph2cart(deg2rad(lonCoords(:)),deg2rad(latCoords(:)), R);
       buildings(index_buildID).x = x;
       buildings(index_buildID).y = y;
       buildings(index_buildID).cX = mean(x);
       buildings(index_buildID).cY = mean(y);
    end

end
