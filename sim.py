import subprocess
import tempfile
import xlsxwriter
import sys

filename = 'test_data.xlsx'

row_headings = [
    ['25', '25'],
    ['50', '50'],
]

col_headings = [
    'Loop zero runtime average (seconds)', 'Loop one runtime average (seconds)', 'Loop two runtime average (seconds)',
    'Loop three runtime average (seconds)', 'Total runtime average (seconds)', 'Average GFLOPs', 'Average GB/s',
    'Average answer'
]

with tempfile.TemporaryFile() as tempf:
  #setup
  data_to_write = []
  data_to_write.append([""] + col_headings)

  #placing in actual data
  for row_heading in row_headings:
    next_row = [row_heading[0] + " " + row_heading[1]]
    #getting results for each setting
    print("Getting average results for stars:{0}, timesteps:{1}".format(
        row_heading[0], row_heading[1]))
    proc = subprocess.Popen(
        ['./script_formatted', '5'] + row_heading, stdout=tempf)
    proc.wait()
    tempf.seek(0)
    output = tempf.read().decode("utf-8").split('\n')
    output = output[:-1]
    print(output)
    for line in output:
      next_row.append(str(float(line.split(":")[1])))

    # print(next_row)
    data_to_write.append(next_row)

  workbook = xlsxwriter.Workbook(filename)
  worksheet = workbook.add_worksheet()

  col_i = 0
  row_i = 0

  print("Writing to spreadsheet")

  for row in data_to_write:
    for cell in row:
      worksheet.write(row_i, col_i, cell)
      col_i += 1
    col_i = 0
    row_i += 1

  workbook.close()
