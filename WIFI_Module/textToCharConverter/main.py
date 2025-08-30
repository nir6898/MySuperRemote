
# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    input_HTML = open("HTML_page.txt", "r")
    output_char_vector = open(r"C:\Users\nir vegh\Desktop\MySuperRemote\WIFI_Module\Basic_Wifi_Connection\HTML_page.h", "w")
    for line in input_HTML:
        output_char_vector.write("client.print(\"")
        output_char_vector.write(line.replace("\n", ""))
        output_char_vector.write("\");\n")

    output_char_vector.close()


# See PyCharm help at https://www.jetbrains.com/help/pycharm/
