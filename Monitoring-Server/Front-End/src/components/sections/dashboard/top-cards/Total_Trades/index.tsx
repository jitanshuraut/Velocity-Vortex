import Box from '@mui/material/Box';
import Stack from '@mui/material/Stack';
import Paper from '@mui/material/Paper';
import Typography from '@mui/material/Typography';
import IconifyIcon from 'components/base/IconifyIcon';
import ClientChart from './ClientChart';

interface TotalTradesProps {
  trades: number;
}

const Total_Trades: React.FC<TotalTradesProps> = ({ trades }) => {
  return (
    <Paper
      component={Stack}
      alignItems="center"
      justifyContent="space-between"
      sx={{ px: 2, py: 2.5 }}
    >
      <Stack alignItems="center" justifyContent="flex-start" spacing={2}>
        <Stack
          alignItems="center"
          justifyContent="center"
          height={56}
          width={56}
          sx={(theme) => ({
            background: `linear-gradient(135deg, ${theme.palette.gradients.primary.state} 0%, ${theme.palette.gradients.primary.main} 100%)`,
          })}
          borderRadius="50%"
        >
          <IconifyIcon icon="weui:done-filled" color="info.light" fontSize="h3.fontSize" />
        </Stack>
        <Box>
          <Typography variant="body2" color="text.disabled" fontWeight={500}>
            Total Trades
          </Typography>
          <Typography mt={0.5} variant="h4">
            {trades}
          </Typography>
        </Box>
      </Stack>

      <ClientChart data={[15, 12, 50, 45, 60]} sx={{ width: 75, height: '68px !important' }} />
    </Paper>
  );
};

export default Total_Trades;
