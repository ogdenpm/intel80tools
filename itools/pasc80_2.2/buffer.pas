program example;

{ Example using bufferread and bufferwrite with break characters }

var buffer: string;
    disk_storage: file;
    break: char;
    new_len, len: integer;
    buff_array: packed array[0..80] of char;

begin
  rewrite (disk_storage, 'data');
  writeln('Input a line of text: ');
  readln (buffer);
  len := bufferwrite(disk_storage, buffer[1], length(buffer));
  repeat
    reset(disk_storage);
    writeln; writeln;
    write('Input break char [cntrl Z to stop]: ');
    readln(break);
    if not eof(input) then
      begin
        new_len := bufferread(disk_storage, buff_array, len, ord(break));
        writeln('The buffer read: ');
        writeln(copy(buffer, 1, abs(new_len)));
        writeln('Length: ', abs(new_len):0);
        if new_len < 0 then writeln('(Break char not found)');
      end;
  until eof(input);
end.
