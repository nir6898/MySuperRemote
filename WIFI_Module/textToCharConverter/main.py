
# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    input_HTML = open("HTML_style.txt", "r")
    output_char_vector = open(r"C:\Users\nir vegh\Desktop\MySuperRemote\WIFI_Module\Basic_Wifi_Connection\HTML_style.h", "w")
    # output_char_vector = open("out_test", "w")
    output_char_vector.write("char HTML_style[] = \"")
    for line in input_HTML:
        output_char_vector.write(line[:-1].replace("\"", "'"))

    output_char_vector.write("\";")
    output_char_vector.close()


# See PyCharm help at https://www.jetbrains.com/help/pycharm/
