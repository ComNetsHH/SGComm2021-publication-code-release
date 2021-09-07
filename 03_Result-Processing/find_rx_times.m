function [rx_times] = find_rx_times(trace,transmitter,num_nodes)

    rx_times = zeros(1,num_nodes);
    for node = 1:num_nodes
        if(node~=transmitter)
            % all node events
            node_events = trace(str2double(trace(:,3)) == node,:);
            % find RX ND event
            rx_event = find(strcmp(node_events(:,4),'RX') & strcmp(node_events(:,5),'ND'),1);
            if(isempty(rx_event))
                rx_event = find(strcmp(node_events(:,4),'RX') & strcmp(node_events(:,5),'D'),1);
            end
            if(isempty(rx_event))
                rx_event = find(strcmp(node_events(:,4),'RX') & strcmp(node_events(:,5),'BB-D'),1);
            end
            if(~isempty(rx_event))
                rx_times(node) = str2double(node_events(rx_event,1));
            else
                return;
            end
        end
    end
end

