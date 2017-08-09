function dims = parse_lavision_ome_xml(xml,dims)

    % The following avoids the need for file I/O:
    inputObject = java.io.StringBufferInputStream(xml);  % or: org.xml.sax.InputSource(java.io.StringReader(xmlString))
    try
        % Parse the input data directly using xmlread's core functionality
        parserFactory = javaMethod('newInstance','javax.xml.parsers.DocumentBuilderFactory');
        p = javaMethod('newDocumentBuilder',parserFactory);
        dom = p.parse(inputObject);
    catch
        % Use xmlread's semi-documented inputObject input feature
        dom = xmlread(inputObject);
    end
    
    function element = get_element(name)
        element = [];
        el = dom.getElementsByTagName(name);
        if el.getLength() > 0
            el = el.item(0);
            attr = el.getAttributes();
            for i=1:attr.getLength()
                attr.item(i-1).getName();
                name = matlab.lang.makeValidName(char(attr.item(i-1).getName()));
                element.(name) = char(attr.item(i-1).getValue());
            end
        end
    end

    if isempty(get_element('ImspectorVersion'))
        throw(MException('FLIMfit:errorProcessingLavision','Not a LaVision file'));
    end
    
    % Detect time gated file
    fast_delay = get_element('Fast_Delay_Box_Is_Active');

    % Detect DC-TCSPC
    dc_tcspc = get_element('DC-TCSPC_Is_Active');

    
    % Gated
    if ~isempty(fast_delay) && str2double(fast_delay.Value) == 1
        delay_values = get_element('Fast_Delay_Box_T_List_of_Values');
        if isempty(delay_values)
            throw(MException('FLIMfit:errorProcessingLavision','Could not find fast delay values'));
        end
        
        dims.delays = str2double(delay_values.Value);
        dims.FLIM_type = 'Gated';
        
    % TCSPC
    elseif ~isempty(dc_tcspc) && str2double(dc_tcspc.Value) == 1
      
        lifetime_axis = [];
        for ax=1:3
            axis = get_element(['Axis' num2str(ax-1)]);
            if ~isempty(axis)
                if strcmp(axis.AxisName,'lifetime')
                    axis.Number = ax-1;
                    lifetime_axis = axis;
                end
            end
        end
        
        if isempty(lifetime_axis) % assume Z, old style
            pixels = get_element('Pixels');
            lifetime_axis.Steps = dims.sizeZCT(1);
            lifetime_axis.PhysicalUnit = str2double(pixels.PhysicalSizeZ); 
        end
        
        steps = str2double(lifetime_axis.Steps);
        unit = str2double(lifetime_axis.PhysicalUnit);
        
        dims.delays = (0:(steps-1)) * unit * 1e3; % ns->ps
        dims.FLIM_type = 'TCSPC';        
    end
    
    modulo_options = 'ZCT';        
    idx=find(dims.sizeZCT == length(dims.delays),1);
    dims.modulo = ['ModuloAlong' modulo_options(idx)];
end