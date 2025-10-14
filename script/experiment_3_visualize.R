source("script/general.R")
l_tex <- FALSE

print_best <- function(data, name = "strided"){
    result <- data[0,]
    
    for(i in unique(data$thread_count)){
        sub <- data[data$thread_count == i,]
        a <- sub[sub$algorithm %!in% c(name),]
        b <- sub[sub$algorithm %in% c(name),]

        result <- rbind(result, a)
        if(nrow(b) > 0){
            result <- rbind(result, b[b$gibs == max(b$gibs),])
        }
    }
    return(result)
}


main_path <- "results/experiment_multithreading/"
filetype <- "csv"

all_plot_file_data <- get_all_new_files(main_path, filetype, ignore_already_plotted=TRUE)
if(length(all_plot_file_data) == 0){
    cat(paste("No (new) csv file found in ", main_path, "  Skipping...", "\n", sep=""))
}else{
    cat(paste("experiment_fixed_data_size new files found in ", main_path, " plotting:\t", sep = ""))
    setwd(main_path)
    for(file_data in all_plot_file_data){
        cat(paste(paste(file_data$filename, filetype, sep = "."), "\t"))
        raw_data <- fread(paste(file_data$filename, filetype, sep = "."), data.table=FALSE)

        cat(paste(" took ", get_time_string(fsum(raw_data$time_ns)), ", \t", sep = ""))
        to_summarise <- get_aggregation_labels(raw_data)
        raw_data$data_amount <- raw_data$data_amount / raw_data$thread_count # Using data amount as data per thread!!!

        agg_data <- raw_data %>% fgroup_by(algorithm, data_amount, byte_count, thread_count, vector_element_count) %>% fmedian()
        agg_data <- agg_data[agg_data$algorithm %!in% c("dynamic gather"),]
        best_data <- print_best(agg_data)
        best_data <- print_best(best_data, "strided unrolled")
        
        color_label <- color_value(unique(best_data$algorithm))
        y_limits <- c(min(best_data$gibs), max(best_data$gibs))
        
        colnames(best_data)[which(colnames(best_data) == "thread_count")] <- "x_axis"
        pre_plot_data_plus_info <- better_representation_of_few_data_points(best_data)
        pre_plot_data <- pre_plot_data_plus_info$frame
        simple_line_labels <- pre_plot_data_plus_info$info

        x_label <- "Thread Count"
        x_axis <- get_breaks(pre_plot_data$x_axis, 8)
        plot_width <- 4.8
        plot_heigth <- 2.6
        for(option in to_summarise){ 
            plot_data <- pre_plot_data[,which( colnames(pre_plot_data)%in%c("print_name", "x_axis", option))]
            colnames(plot_data)[which(colnames(plot_data) == option)] <- "y_axis"
            y_label <- option 
            if(option == "gibs"){ y_label <- "GiB / s"}

            y_axis <- get_breaks(c(fmin(plot_data$y_axis), fmax(plot_data$y_axis)),6, start_at_zero = TRUE, add_min_value = FALSE)

            lt_values <- linetype_value(plot_data$print_name, simple_line_labels)
            paper_plot(plot_data, paste(file_data$filename, option, sep="/"), 
                list(label = x_label, axis = x_axis),
                list(label = y_label, axis = y_axis), 
                list(label = "", values = lt_values, color = color_label)
                , dimensions = c(2.85, 2.2), multi_line = TRUE, tex=l_tex, y_limits = y_limits #c(2.85, 2.4)
                )
        }
        write_success(file_data)
    }
    cat("\n\n")
}