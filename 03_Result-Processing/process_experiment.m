function [result] = process_experiment(path_to_sim_results,experiment,calculate_peak_aoi)
    % Copy general info
    result.identifier = experiment.identifier;
    result.numSeq = experiment.numSeq;
    result.llTech = experiment.llTech;
    result.name = experiment.name;
    result.rtxTimer = experiment.rtxTimer;
    result.rngID = experiment.rngID;
    result.dynamicRTX = experiment.dynamicRTX;

    rtxTrace = importFile(strcat(path_to_sim_results,"/",experiment.files,"/rtxEventTrace.txt"));
    if(size(rtxTrace,1)>600)
        % Skip first 400 and last 100 updates
        rtxTrace = split(rtxTrace(400:end-100,1),':');
        mcdsNodes = unique(str2double(rtxTrace(:,2)));
        for mcdsNode_id = 1:size(mcdsNodes,1)
            result.rtxMean(mcdsNode_id) = mean(str2double(rtxTrace(rtxTrace(:,2)==num2str(mcdsNodes(mcdsNode_id)),3)));
        end
    else
        result.rtxMean = 0;
    end

    trace = importFile(strcat(path_to_sim_results,"/",experiment.files,"/eventTrace.txt"));

    % Cut start info
    info_end = find(strcmp(trace,"Positions End")==1);
    trace = trace(info_end+2:end,:);
    trace = [split(trace(:,1)," "),trace(:,2:end)];


    % Extract general info
    num_nodes = max(str2double(trace(:,3)));
    t_start = str2double(trace(1,1));
    t_end = str2double(trace(end,1));
    unique_info = unique(trace(:,6),'stable');


    % Handle each (node,seqNum) tuple individually
    for infoID = 1:length(unique_info)
        info = split(unique_info(infoID),',');
        transmitter = str2double(erase(info(1),'('));
        seqNum = str2double(erase(info(2),')'));

        tuple_info = trace(trace(:,6)==unique_info(infoID),:);
        result.rxTimes{transmitter,seqNum} = find_rx_times(tuple_info,transmitter,num_nodes);
        result.txTime(transmitter,seqNum) = str2double(tuple_info(1,1));
        result.etx(infoID) = nnz([contains(tuple_info(:,4),"TX")]);
    end

    peakCalcBasis.info = "";

    % result.rxTimes = cell at position [n x k] reception times for fragment (n,k) 
    % Calculate global statistics e.g. Aoi
    for transmitter = 1:num_nodes
        if(size(result.rxTimes,1)==num_nodes)
            rx_events = vertcat(result.rxTimes{transmitter,:});
            tx_events = vertcat(result.txTime(transmitter,:));
            for receiver = 1:num_nodes
                t2 = [];
                delay2h = [];
                delay2l = [];
                delay2i = [];
                if(receiver~=transmitter)
                    t2 = repelem(rx_events(2:end-1,receiver),2,1);
                    delay2h = diff(rx_events(1:end-1,receiver)) + rx_events(2:end-1,receiver);
                    delay2h = delay2h-tx_events(2:2+size(delay2h,1)-1)';
                    delay2l = rx_events(2:end-1,receiver);
                    delay2l = delay2l-tx_events(2:2+size(delay2h,1)-1)';
                    for ii = numel(delay2h):-1:1
                        delay2i(2*ii-[0 1]) = [delay2l(ii),delay2h(ii)];
                    end
                    delay2 = delay2i;
                else
                    t2 = repelem(zeros(size(rx_events(:,receiver),1),1),2,1);
                    delay2 = NaN(size(t2))';
                end
                peakCalcBasis.t{receiver,transmitter} = t2;
                peakCalcBasis.delay2{receiver,transmitter} = delay2;

                result.AoI(receiver,transmitter).mean = trapz(t2,delay2)/(max(t2)-min(t2));
            end
        end
        result.rtxShare(transmitter) = nnz([str2double(trace(:,3))==transmitter] & [trace(:,4) == "RTX"]);
    end
    result.avgAoI = mean([result.AoI(:).mean],'omitnan');

    if(calculate_peak_aoi)
        for receiver = 1:num_nodes
            t = peakCalcBasis.t(receiver,:);
            delay = peakCalcBasis.delay2(receiver,:);
            buffer = sparse(zeros(num_nodes,t_end-t_start));
            for transmitter = 1:num_nodes
                if(transmitter ~= receiver)
                tt = t{transmitter};
                delayt = delay{transmitter};
                buffer(transmitter,[tt(1:2:end);tt(2:2:end)+1]) = [delayt(1:2:end),delayt(2:2:end)+1];
                end
            end
            tt = [vertcat(t{:})];
            tt = tt(1:2:end)+1;
            tSkip = t_start+2*max([delay{1:end-1}],[],'omitnan');
            peakAges = zeros(1,size(tt,1));
            peakAgeTransmitter = zeros(1,size(tt,1));

            for toi_id = 1:size(tt,1)
               toi=tt(toi_id);
               maxAge = 0;
               maxTransmitter = 0;
               if toi > tSkip
                   maxAge = 0;
                   maxTransmitter = 0;
                   for transmitter = 1:num_nodes
                        group_start_idx = find(buffer(transmitter,1:toi) > 0, 1, 'last') + 1;
                        age = buffer(transmitter,group_start_idx-1)+(toi-group_start_idx);
                        if(age>maxAge)
                            maxAge=age;
                            maxTransmitter = transmitter;
                        end
                   end
               end
               peakAges(toi_id) = maxAge;
               peakAgeTransmitter(toi_id) = maxTransmitter;
            end
            peakAges(peakAges==0) = NaN;
            result.peakAvgAoI(receiver) = mean(peakAges,'omitnan');
        end
    else
        for receiver = 1:num_nodes
             result.peakAvgAoI(receiver) = 0;
        end
    end

end


