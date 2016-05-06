import javax.swing.filechooser.*;
import java.io.File;

public class StatView_FileFilter extends FileFilter
{
    String description;
    String file_extensions[];

    public StatView_FileFilter(String[] exts, String descr)
    {
	file_extensions = new String[exts.length];
	
	for(int ii = 0; ii < exts.length; ii++)
	    file_extensions[ii] = exts[ii].toLowerCase();
	description = (descr == null ? "files" : descr);
    }

    public boolean accept(File file)
    {
	if(file.isDirectory()){ return true; }
	
	String file_name = file.getName().toLowerCase();
	for(int ii = file_extensions.length-1; ii >= 0;ii--) {
	    if(file_name.endsWith(file_extensions[ii]))
	       return true;
	}
	return false;
    }

    public String getDescription() { return description; }
}


