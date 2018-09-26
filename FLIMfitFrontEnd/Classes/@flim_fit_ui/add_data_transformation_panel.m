function handles = add_data_transformation_panel(obj,handles,parent)

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


   % dataset
    dataset_panel = uix.TabPanel('Parent', parent, 'TabWidth', 80);
 

    % data transformation
    data_layout = uigridlayout('Parent', dataset_panel);
    data_transformation_layout =uigridlayout('Parent', data_layout, 'Spacing', 1, 'Padding', 3 );
    uilabel('Text', 'Smoothing ',       'HorizontalAlignment', 'right', 'Parent', data_transformation_layout);
    uilabel('Text', 'Integrated Min. ', 'HorizontalAlignment', 'right', 'Parent', data_transformation_layout);
    uilabel('Text', 'Time Min. ',    'HorizontalAlignment', 'right', 'Parent', data_transformation_layout);
    uilabel('Text', 'Counts/Photon ',    'HorizontalAlignment', 'right', 'Parent', data_transformation_layout);
    
    handles.binning_popupmenu = uidropdown('Items', {'None', '3x3 (B&H 1)', '5x5 (B&H 2)', '7x7 (B&H 3)' '9x9 (B&H 4)' '11x11 (B&H 5)' '13x13 (B&H 6)' '15x15 (B&H 7)'}, 'Parent', data_transformation_layout);
    handles.thresh_min_edit   = uieditfield('Value', '0', 'Parent', data_transformation_layout);
    handles.t_min_edit        = uieditfield('Value', '0', 'Parent', data_transformation_layout);
    handles.counts_per_photon_edit = uieditfield('Value', '0', 'Parent', data_transformation_layout);
    
    uilabel('Text', 'Rep. Rate ', 'HorizontalAlignment', 'right', 'Parent', data_transformation_layout);
    uilabel('Text', 'Gate Max. ',    'HorizontalAlignment', 'right', 'Parent', data_transformation_layout);
    uilabel('Text', 'Time Max. ',    'HorizontalAlignment', 'right', 'Parent', data_transformation_layout);
    uix.Empty('Parent', data_transformation_layout);
    
    handles.rep_rate_edit          = uieditfield('Value', '0', 'Parent', data_transformation_layout);
    handles.gate_max_edit          = uieditfield('Value', '1e10', 'Parent', data_transformation_layout);
    handles.t_max_edit             = uieditfield('Value', '1e10', 'Parent', data_transformation_layout);
    uix.Empty('Parent', data_transformation_layout);
    
    set(data_transformation_layout,'Heights',[22 22 22 22]);
    set(data_transformation_layout,'Widths',[120 120 120 120]);   
    
      
    handles.background_container = uipanel('Parent', data_layout); 
    handles.background_axes = axes('Parent', handles.background_container);
    
    set(data_layout,'Heights',[150 -1]);
    
    % background
    background_layout = uigridlayout('Parent', dataset_panel);
    background_layout =uigridlayout('Parent', background_layout, 'Spacing', 1, 'Padding', 3 );
    
    uilabel('Text', 'Background ', 'HorizontalAlignment', 'right', 'Parent', background_layout);
    uilabel('Text', 'Background Value ', 'HorizontalAlignment', 'right', 'Parent', background_layout);
    
    handles.background_type_popupmenu = uidropdown('Items', {'None', 'Single Value', 'Image', 'TV Intensity Map'}, 'Parent', background_layout);
    handles.background_value_edit = uieditfield('Value', '0', 'Parent', background_layout);
    %handles.tvb_define_pushbutton = uibutton('Text', 'Define', 'Parent', background_layout);
    
    
    set(background_layout,'Heights',[22 22]);
    set(background_layout,'Widths',[120 120]);
    
    % irf transformation
    irf_layout = uigridlayout('Parent', dataset_panel);
    irf_transformation_layout =uigridlayout('Parent', irf_layout, 'Spacing', 1, 'Padding', 3);

    uilabel('Text', 'IRF Type ', 'HorizontalAlignment', 'right', 'Parent', irf_transformation_layout);
    uilabel('Text', 'Background ', 'HorizontalAlignment', 'right', 'Parent', irf_transformation_layout);
    uilabel('Text', 'Time Min. ', 'HorizontalAlignment', 'right', 'Parent', irf_transformation_layout);
    uilabel('Text', 'IRF Shift ', 'HorizontalAlignment', 'right', 'Parent', irf_transformation_layout);
    
    handles.irf_type_popupmenu = uidropdown('Items', {'Scatter', 'Reference'}, 'Parent', irf_transformation_layout);
    handles.irf_background_edit = uieditfield('Value', '0', 'Parent', irf_transformation_layout);
    handles.t_irf_min_edit = uieditfield('Value', '0', 'Parent', irf_transformation_layout);
    handles.t0_edit = uieditfield('Value', '0', 'Parent', irf_transformation_layout);    
    
    
    uilabel('Text', 'Reference Lifetime ', 'HorizontalAlignment', 'right', 'Parent', irf_transformation_layout);
    uilabel('Text', 'BG is Afterpulsing ', 'HorizontalAlignment', 'right', 'Parent', irf_transformation_layout);
    uilabel('Text', 'Time Max. ', 'HorizontalAlignment', 'right', 'Parent', irf_transformation_layout);
    uix.Empty('Parent', irf_transformation_layout);
    
    
    handles.ref_lifetime_edit = uieditfield('Value', '80', 'Parent', irf_transformation_layout);
    handles.afterpulsing_correction_popupmenu = uidropdown('Items', {'No', 'Yes'}, 'Parent', irf_transformation_layout);
    handles.t_irf_max_edit = uieditfield('Value', '1e10', 'Parent', irf_transformation_layout);
    uix.Empty('Parent', irf_transformation_layout);
    
    
    set(irf_transformation_layout,'Heights',[22 22 22 22]);
    set(irf_transformation_layout,'Widths',[120 120 120 120]);

    
    % polarisation
    pol_layout = uigridlayout('Parent', dataset_panel, 'Spacing', 1, 'Padding', 3 );

    pol_left_layout =uigridlayout('Parent', pol_layout, 'Spacing', 1, 'Padding', 0);

    uilabel('Text', 'G Factor ', 'HorizontalAlignment', 'right', 'Parent', pol_left_layout);
    uilabel('Text', 'Polarisation Angle ', 'HorizontalAlignment', 'right', 'Parent', pol_left_layout);
    handles.g_factor_edit = uieditfield('Value', '1', 'Parent', pol_left_layout);
    handles.pol_angle_edit = uieditfield('Value', '1', 'Parent', pol_left_layout);
    
    
    handles.pol_table = uitable('Parent', pol_layout,... 
        'Data', {'0','Unpolarised'},...
        'ColumnWidth', {120 120},...
        'ColumnName', {'Channel','Polarisation'},...
        'ColumnFormat', {'numeric',{'Unpolarised','Parallel','Perpendicular'}},...
        'ColumnEditable', [false, true],...
        'RowName',[]);  
    
    set(pol_left_layout,'Heights',[22 22]);
    set(pol_left_layout,'Widths',[120 120]);

    
    set(pol_layout,'Widths',[241 -1]);   
    
    set(dataset_panel, 'TabTitles', {'Data'; 'Background'; 'IRF'; 'Polarisation'});
    set(dataset_panel, 'Selection', 1);

end