 classdef flim_fit_plot_controller < flim_fit_observer
   
    properties
        plot_select_table;
        plot_panel;

        dataset_selected = 1;
        
        n_plots = 0;
        display_normal = struct();
        display_merged = struct();
        plot_names = {};
        plot_data;
        default_lims = {};
        plot_lims = struct();
        auto_lim = struct();
        
        invert_colormap_popupmenu;
        
        data_series_list;
        lh = {};
    end
    
    properties(Access='protected')
        n_exp_list = 0;
        n_fret_list = 0;
        inc_donor_list = 0;
    end
    
    methods
       
        function obj = flim_fit_plot_controller(handles)
                       
            obj = obj@flim_fit_observer(handles.fit_controller);
            
            assign_handles(obj,handles);

            set(obj.plot_select_table,'CellEditCallback',@obj.plot_select_update);
            
            addlistener(obj.plot_panel,'Position','PostSet',@obj.panel_resized);
            addlistener(obj.data_series_list,'selection_updated',@obj.dataset_selected_update);

            add_callback(obj.invert_colormap_popupmenu,@(~,~,~) obj.update_plots);
            
            obj.update_list();
            obj.update_table();
            
        end
        
        function export_plots(obj,file_root)
            obj.update_plots(file_root);
        end
        
        
        function panel_resized(obj,~,~)
            obj.update_plots();
        end
        
        function lims = get_lims(~,var)
            var = var(:);
            lims = [min(var) max(var)];
        end
        
        
        function dataset_selected_update(obj,src,~)
                       
            obj.dataset_selected = src.use_selected;
            obj.update_plots();
        end
        
        function plot_select_update(obj,~,~)
            plots = get(obj.plot_select_table,'Data');

            obj.n_plots = 0;
            
            for i=1:size(plots,1)
               name = plots{i,1};
               obj.display_normal.(name) = plots{i,2};
               obj.display_merged.(name) = plots{i,3};
               
               obj.n_plots = obj.n_plots + sum(cell2mat(plots(i,2:3))); 
               
               new_lims = cell2mat(plots(i,4:5));
               if any(new_lims ~= obj.plot_lims.(name))
                   obj.auto_lim.(name) = false;
               else
                   obj.auto_lim.(name) = plots{i,6};
               end
               obj.plot_lims.(name) = new_lims;
            end
            
            obj.update_table();
            obj.update_plots();
            
        end
        
        function fit_update(obj)
            if ishandle(obj.plot_panel) %check object hasn't been closed
                obj.update_list();
                obj.update_table();
                obj.update_plots();
            end
        end
        
        function update_table(obj)
                       
            if obj.fit_controller.has_fit
                r = obj.fit_controller.fit_result;
                names = obj.plot_names;
                table = cell(length(names),6);
                for i=1:length(names)
                    
                    if obj.auto_lim.(names{i}) 
                        obj.plot_lims.(names{i}) = r.get_default_lims(i);
                    end
                    
                    table{i,1} = names{i};
                    table{i,2} = obj.display_normal.(names{i});
                    table{i,3} = obj.display_merged.(names{i});
                    table(i,4:5) = num2cell(obj.plot_lims.(names{i}));
                    table{i,6} = obj.auto_lim.(names{i});
                end
                
                for i=1:length(names) 
                    r.set_cur_lims(i, obj.plot_lims.(names{i}));
                end
                
                set(obj.plot_select_table,'Data',table);
                set(obj.plot_select_table,'ColumnEditable',logical([0 1 1 1 1 1]));
                set(obj.plot_select_table,'RowName',[]);
                
            end
        end
                
    end
    
end