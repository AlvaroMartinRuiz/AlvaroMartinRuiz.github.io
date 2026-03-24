library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity GenSen is
  port(
    Clk   : in  std_logic;                 -- clock (100 MHz )
    Reset : in  std_logic;                 -- Async reset (active high)
    per   : in  std_logic_vector(1 downto 0);  -- For seecting the frequency 
    led   : out signed(7 downto 0);        -- Signed output for LEDs
    dac   : out unsigned(7 downto 0)       -- Unsigned output for DAC (always positive because we have to add 128)
  );
end GenSen;

architecture Behavioral of GenSen is

  --------------------------------------------------------------------------
  -- Signal Declarations
  
  signal sample_counter : unsigned(3 downto 0) := (others => '0'); 
    -- Points to the current sine sample [0..15]

  signal timer_count : unsigned(13 downto 0) := (others => '0');
    -- Counts clock cycles until we reach MaxCount (when MaxCount is reahced, it resets).
	-- Our min frequency is 500 Hz, as clk has 100 Mhz.  The max possible value for timer_count and 
	-- MaxCount is 100Mhz/(500 * 16) = 12500, so we only need 14 bits  

  signal MaxCount : unsigned(13 downto 0) := (others => '0'); -- todo: preguntar al profe si ponemos mas de 13 bits
    -- Determines how many clock cycles to wait before switching to the next sample (so it depends on 'per')

  signal sin_val : signed(7 downto 0) := (others => '0');
    -- Current sine sample from the ROM

begin

  --------------------------------------------------------------------------
  -- Instantiate the Sine ROM
  
  sine_memory : entity work.sine_rom -- 'work' is the library of work of VHDL
    port map(
      addr => sample_counter,
      dout => sin_val
    );

  --------------------------------------------------------------------------
  -- Asynchronous reset
  
  process(Clk, Reset) -- this is a process because it has to be executed sequentially
  begin
    if Reset = '1' then -- active high
      sample_counter <= (others => '0');
      timer_count     <= (others => '0');
    elsif rising_edge(Clk) then

      ----------------------------------------------------------------------
      -- Count clock cycles for one sample
      
      if timer_count = MaxCount then
        timer_count <= (others => '0');
        -- Move to next sample
        sample_counter <= sample_counter + 1;
      else
        timer_count <= timer_count + 1;
      end if;

    end if; -- rising_edge
  end process;

  --------------------------------------------------------------------------
  -- Select the MaxCount (to adapt the frquency) depending on 'per'
  
  with per select
    MaxCount <= 
      "11000011010100" when "00",  -- 12500 decimal -> 500 Hz  
      "01100001101010" when "01",  -- 6250 decimal -> 1000 Hz 
      "00101100011001"   when "10",  -- 2841 decimal -> 2200 Hz 
      "00011000011011"   when others;-- 1563 decimal -> 4000 Hz 



  --------------------------------------------------------------------------
  -- Outputs:
  led <= sin_val;

  dac <= unsigned(sin_val) + 128;  -- add the offset of 128, select only  8 bits

end Behavioral;
