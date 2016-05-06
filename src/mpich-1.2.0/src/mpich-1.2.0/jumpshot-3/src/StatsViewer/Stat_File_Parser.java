import java.io.*;
import java.util.*;

public class Stat_File_Parser
{
    private RandomAccessFile file_handle;
    private int line_number;
    private String x_labels[],
	           y_labels[],
	           title,
	           input;
    private double data[][] = new double[0][0];
    private char delimiter = '\t';

    public Stat_File_Parser(String file_name)
	throws IOException
    {
	file_handle = new RandomAccessFile(file_name,"r");
	input = null;
	line_number = 0;
    }
    
    public Stat_File_Parser(String file_name, char c)
        throws IOException
    {
	this(file_name);
	delimiter = c;
    }

    public Statistics_Data readGraphData()
    throws IOException
    {
	int num_x_values, 
	    return_value;

	x_labels = new String[0];
	y_labels = new String[0];
	title = "";
	data = new double[0][0];
	Vector dataSwapper = new Vector();

	
	try {
	    if(input == null)
		input = file_handle.readLine();
	}
	catch(IOException e) {
	    System.out.println("File_Parser:30:" + e.getMessage());
	    throw e;
	}
	
	if(input == null) {
	    file_handle.close();
	    return null;
	}


	line_number++;
	num_x_values = processTitleAndXLabels(input);
	
	if(num_x_values == 0) {
	    file_handle.close();
	    return null;
	}
	
	try {
	    input = file_handle.readLine();
	    line_number++;
	}

	catch(IOException e) {
	    System.out.println("File_Parser: Unable to read line " + 
			       line_number + " from file.\n");
	    try {
		file_handle.close();
	    }
	    catch(IOException ee){}
	    throw e;
	}

	while(input != null) {
	    if(Character.isLetterOrDigit(input.charAt(0))) {
		try {
		    return_value = 
			processData(input, num_x_values,dataSwapper);
		}
		catch(NumberFormatException e) {
		    System.out.println("FileParser: Data values not in" +
				       " correct format.\n");
		    try {
			file_handle.close();
		    }
		    catch(IOException ee){}
		    throw new IOException();
		}
		if(return_value != num_x_values){
		    System.out.println("FileParser: Inconsistent data. Line" +
				       " number " + line_number);
		    try {
			file_handle.close();
		    }
		    catch(IOException ee){}
		    throw new IOException();
		}
	
		try {
		    input = file_handle.readLine();
		    line_number++;
		}
		catch(IOException e) {
		    System.out.println("File_Parser: Unable to read line " + 
				       line_number + " from file.\n");
		    try {
			file_handle.close();
		    }
		    catch(IOException ee){}
		    throw new IOException();
		} 
	    }
	    else break;
	    	
	}
	
	int ii,kk;

	if(dataSwapper.size() > 0)
	    data = new double[((double[])dataSwapper.elementAt(0)).length]
		[dataSwapper.size()];
	
	for(ii = 0; ii < data.length; ii++)
	    for(kk = 0; kk < (data[ii].length); kk++)
		data[ii][kk] = ((double[])dataSwapper.elementAt(kk))[ii];

	return new Statistics_Data(title, data, y_labels, x_labels);
    }

    private int processTitleAndXLabels(String input)
    {
	int str_parser = 0,
	    num_x_values = 0;
	char string_buffer[] = new char[input.length()];
	String x_labels_swap_aray[];
	
	while(str_parser < input.length()) {
	    if(Character.isWhitespace(input.charAt(str_parser)))
		str_parser++;
	    else break;
	}

	int ii=0;
	while(str_parser < input.length()) {
	    if(input.charAt(str_parser) != delimiter)
		string_buffer[ii++] = input.charAt(str_parser++);
	    else break;
	}

	if(string_buffer.length > 0)	
	    title = new String(string_buffer,0,ii);

	while(str_parser < input.length()) {
	    if(Character.isWhitespace(input.charAt(str_parser)))
		str_parser++;
	    else break;
	}

	while(str_parser < input.length()) {
	    ii=0;
	    while(str_parser < input.length()) {
		if(input.charAt(str_parser) != delimiter)
		    string_buffer[ii++] = input.charAt(str_parser++);
		else break;
	    }  

	    if(!Character.isWhitespace(input.charAt(str_parser-1))) {
		num_x_values++;
		x_labels_swap_aray = new String[x_labels.length+1];
		int kk;
		for(kk=0; kk<x_labels.length;kk++)
		    x_labels_swap_aray[kk] = x_labels[kk];
		x_labels_swap_aray[kk] = new String(string_buffer,0,ii);
		x_labels = x_labels_swap_aray;
	    }

	    if(str_parser < input.length())
		str_parser++;
	}
	return num_x_values;
    }
    
    private int processData(String input, int num_vals, Vector dataSwapper)
    throws NumberFormatException
    {
	int str_parser = 0,
	    num_x_values = 0;
	char string_buffer[] = new char[input.length()];
	String temp_string,
	       temp_str_aray[];
	double temp_data_array[];
	int ii=0;

	while(str_parser < input.length()) {
	    if(Character.isWhitespace(input.charAt(str_parser)))
		str_parser++;
	    else break;
	}

	while(str_parser < input.length()) {
	    if(input.charAt(str_parser) != delimiter)
		string_buffer[ii++] = input.charAt(str_parser++);
	    else break;
	}

	temp_str_aray = new String[y_labels.length+1];

	int kk;
	for(kk=0; kk < y_labels.length; kk++)
	    temp_str_aray[kk] = y_labels[kk];

	temp_str_aray[kk] = new String(string_buffer,0,ii);
	y_labels = temp_str_aray;

	while(str_parser < input.length()) {
	    if(Character.isWhitespace(input.charAt(str_parser)))
		str_parser++;
	    else break;
	}
	
	temp_data_array = new double[num_vals];
	while(str_parser < input.length()) {
	    ii=0;
	    while(str_parser < input.length()) {
		if(input.charAt(str_parser) != delimiter)
		    string_buffer[ii++] = input.charAt(str_parser++);
		else break;
	    }  
	    
	    if((!Character.isWhitespace(input.charAt(str_parser-1)))
	       && (num_x_values < num_vals)) {
		temp_string = new String(string_buffer,0,ii);
		temp_data_array[num_x_values] =  
		    Double.valueOf(temp_string).doubleValue();
		num_x_values++;
	    }
	    if(str_parser < input.length())
		str_parser++;
	}
	dataSwapper.addElement(temp_data_array);
	return num_x_values;
    }

}
	
	
	
	
	    
		
	    
	       
