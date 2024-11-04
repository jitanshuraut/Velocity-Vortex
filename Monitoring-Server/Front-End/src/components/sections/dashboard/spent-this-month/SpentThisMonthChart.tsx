import { useMemo } from 'react';
import { SxProps, useTheme } from '@mui/material';
import * as echarts from 'echarts/core';
import ReactEchart from 'components/base/ReactEchart';
import { BarChart } from 'echarts/charts';
import { TooltipComponent, GridComponent, AxisPointerComponent } from 'echarts/components';
import { CanvasRenderer } from 'echarts/renderers';

echarts.use([BarChart, TooltipComponent, GridComponent, AxisPointerComponent, CanvasRenderer]);

interface SpentThisMonthChartProps {
  data: number[];
  sx?: SxProps;
}

const SpentThisMonthChart = ({ data, ...rest }: SpentThisMonthChartProps) => {
  const theme = useTheme();

  const option = useMemo(
    () => ({
      tooltip: {
        axisPointer: null,
        formatter: 'Spent: ${c}',
        borderWidth: 0,
      },
      grid: {
        top: '22%',
        left: '5%',
        right: '5%',
        bottom: '3%',
        containLabel: true,
      },
      xAxis: [
        {
          type: 'category',
          data: ['', '', '', '', '', '', ''],
          boundaryGap: false,
          axisTick: {
            show: false,
          },
          axisLine: {
            show: false,
          },
          axisLabel: {
            margin: 15,
            fontWeight: 500,
            color: theme.palette.text.disabled,
            fontSize: theme.typography.caption.fontSize,
            fontFamily: theme.typography.fontFamily,
          },
        },
      ],
      yAxis: [
        {
          type: 'value',
          min: 10,
          minInterval: 1,
          axisLabel: {
            show: false,
          },
          splitLine: {
            show: false,
          },
        },
      ],
      series: [
        {
          name: 'Spent',
          type: 'bar',
          barWidth: '35%',
          showBackground: true,
          backgroundStyle: {
            color: theme.palette.info.dark,
            borderRadius: [10, 10, 10, 10],
          },
          data,
          itemStyle: {
            color: theme.palette.primary.main,
            borderRadius: [10, 10, 10, 10],
          },
          emphasis: {
            itemStyle: {
              color: theme.palette.primary.main,
            },
          },
        },
      ],
    }),
    [theme, data],
  );

  return <ReactEchart echarts={echarts} option={option} {...rest} />;
};

export default SpentThisMonthChart;
