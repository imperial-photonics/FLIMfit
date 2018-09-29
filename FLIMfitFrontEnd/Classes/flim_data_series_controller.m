%> @ingroup UserInterfaceControllers
classdef flim_data_series_controller < handle 
    
    % Copyright (C) 2013 Imperial College London.
    % All rights reserved.
    %
    % This program is free software; you can redistribute it and/or modify
    % it under the terms of the GNU General Public License as published by
    % the Free Software Foundation; either version 2 of the License, or
    % (at your option) any later version.
    %
    % This program is distributed in the hope that it will be useful,
    % but WITHOUT ANY WARRANTY; without even the implied warranty of
    % MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    % GNU General Public License for more details.
    %
    % You should have received a copy of the GNU General Public License along
    % with this program; if not, write to the Free Software Foundation, Inc.,
    % 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    %
    % This software tool was developed with support from the UK 
    % Engineering and Physical Sciences Council 
    % through  a studentship from the Institute of Chemical Biology 
    % and The Wellcome Trust through a grant entitled 
    % "The Open Microscopy Environment: Image Informatics for Biological Sciences" (Ref: 095931).

    % Author : Sean Warren

    
    properties(SetObservable = true)
        data_series;
        fitting_params_controller;
        model_controller;
        display_smoothed_popupmenu;
        window;
        version;
    end
    
    events
        new_dataset;
    end
    
    methods
        
        function obj = flim_data_series_controller(varargin)
            
            handles = args2struct(varargin);
            assign_handles(obj,handles);
            
            if isempty(obj.data_series) 
                obj.data_series = flim_data_series();
            end
            
            set(obj.display_smoothed_popupmenu,'ValueChangedFcn',@obj.set_use_smoothing);
            
        end
        
        function set_use_smoothing(obj,src,~)
            obj.data_series.use_smoothing = src.Value == 2;
            obj.data_series.compute_tr_data();
        end
        
        function file_name = save_settings(obj)
            if isvalid(obj.data_series)
                file_name = obj.data_series.save_data_settings();
            end
        end
        
        function clear_data_series(obj)
            delete(obj.data_series);
            obj.data_series = flim_data_series();
        end
        
        function load_data_series(obj,root_path,mode,polarisation_resolved)
            if nargin < 4
                polarisation_resolved = false;
            end

            obj.clear_data_series();
            obj.data_series.load_data_series(root_path,mode,polarisation_resolved);
                        
            if ~isempty(obj.window)
                set(obj.window,'Name',[ obj.data_series.header_text ' (' obj.version ')']);
            end

            obj.loaded_new_dataset();
        end
        
        function load_raw(obj,file)
            obj.clear_data_series();
            obj.data_series.load_raw_data(file);           
                       
            if ~isempty(obj.window)
                set(obj.window,'Name',[file ' (' obj.version ')']);
            end
            
            obj.loaded_new_dataset();
        end
        
        
        function load_single(obj,file,polarisation_resolved)
            % save settings from previous dataset if it exists
            if nargin < 3
                polarisation_resolved = false;
            end

            % load new dataset
            obj.clear_data_series();
            obj.data_series.load_single(file,polarisation_resolved);
                        
            if ~isempty(obj.window)
                set(obj.window,'Name',[obj.data_series.header_text ' (' obj.version ')']);
            end
                        
            obj.loaded_new_dataset();
        end
        
        function load_plate(obj,plate)
                   
            % load new dataset
            obj.data_series.load_plate(plate);
            
            if ~isempty(obj.window)
                set(obj.window,'Name',[obj.data_series.header_text ' (' obj.version ')']);
            end
                        
            obj.loaded_new_dataset();
        end
        
        function loaded_new_dataset(obj)
            obj.model_controller.set_n_channel(obj.data_series.n_chan);
            notify(obj,'new_dataset');
        end

        function intensity = selected_intensity(obj,selected,apply_mask)
            if nargin == 2
                apply_mask = true;
            end
            
            if obj.data_series.init && selected > 0 && selected <= obj.data_series.n_datasets
                intensity = obj.data_series.selected_intensity(selected,apply_mask);
            else
                intensity = [];
            end
        end
        
        function mask = selected_mask(obj,selected)
           
            mask = [];
            
            if obj.data_series.init && selected > 0 && selected <= obj.data_series.n_datasets
                mask = obj.data_series.mask(:,:,selected);
                if ~isempty(obj.data_series.seg_mask)
                    seg_mask = obj.data_series.seg_mask(:,:,selected);
                    mask = mask & seg_mask;
                end
            end
            
        end
                
    end
end