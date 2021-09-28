# Resume-projects-
<strong>Metadata editor</strong> 

ID3v2 metadata standard.

Executed as a console application accepting a filename as arguments via a parameter --filepath as well as one of the selected commands

1. <em> --show </em> - display all metadata in the table
2. <em> --set = prop_name --value = prop_value </em> - sets the value of a specific metadata field named prop_name in prop_value
3. <em> --get = prop_name </em> - display the value of a field named prop_name

For example:

app.exe --filepath = Song.mp3 --show

app.exe --filepath = Song.mp3 --get = TIT2

<strong> Conway’s Game of Life </strong>

Displays each generation of the game into a monochrome picture of the BMP format. The plane of the "universe" of the game is limited by positive coordinates.

Executed as a console applicationaccepting the following parameters as arguments:
1. <em> --input input_file.bmp </em> - where input_file.bmp is a monochrome picture in bmp format, storing the initial situation (first generation) of the game
2. <em> --output dir_name </em> - the name of the directory for storing generations of the game
3. <em> --max_iter N </em> - maximum number of generations that can emulateprogram. Optional parameter, infinity by default
4. <em> --dump_freq N </em> - the frequency with which the program should keep generations in the form Pictures. Optional parameter, by default equal to 1
